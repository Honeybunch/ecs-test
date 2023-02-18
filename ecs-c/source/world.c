#include "world.h"

#include "allocator.h"
#include "profiling.h"

#include "transformcomponent.h"

void create_component_store(ComponentStore *store,
                            const ComponentDescriptor *desc) {
  store->name = desc->name;
  store->id = desc->id;
  store->id_str = desc->id_str;
  store->size = desc->size;
  store->desc_size = desc->desc_size;
  store->count = 0;
  store->components = NULL;
  store->desc = *desc;
  store->create = desc->create;
  store->on_loaded = desc->on_loaded;
  store->destroy = desc->destroy;
}

bool create_system(World *world, System *system, const SystemDescriptor *desc) {
  const uint32_t dep_count = desc->dep_count > MAX_DEPENDENCY_SET_COUNT
                                 ? MAX_DEPENDENCY_SET_COUNT
                                 : desc->dep_count;

  system->name = desc->name;
  system->id = desc->id;
  system->dep_count = dep_count;
  memcpy(system->deps, desc->deps,
         sizeof(SystemComponentDependencies) * dep_count);
  system->create = desc->create;
  system->destroy = desc->destroy;
  system->tick = desc->tick;

  // Allocate and initialize each system
  system->self = tb_alloc(world->std_alloc, desc->size);

  // If this system has dependencies on other systems, look those up now and
  // pass them to create
  uint32_t system_dep_count = desc->system_dep_count;
  for (uint32_t sys_dep_idx = 0; sys_dep_idx < system_dep_count;
       ++sys_dep_idx) {
    SystemId id = desc->system_deps[sys_dep_idx];
    System *sys = tb_find_system_by_id(world->systems, world->system_count, id);
    system->system_deps[sys_dep_idx] = sys;
  }
  system->system_dep_count = system_dep_count;

  bool created = system->create(system->self, desc->desc,
                                system->system_dep_count, system->system_deps);
  return created;
}

uint32_t find_system_idx_by_id(const SystemDescriptor *descs,
                               uint32_t desc_count, SystemId id) {
  for (uint32_t i = 0; i < desc_count; ++i) {
    const SystemDescriptor *desc = &descs[i];
    if (desc->id == id) {
      return i;
    }
  }
  return 0xFFFFFFFF;
}

bool tb_create_world(const WorldDescriptor *desc, World *world) {
  if (desc == NULL || world == NULL) {
    return false;
  }

  // Assign allocators
  Allocator std_alloc = desc->std_alloc;
  Allocator tmp_alloc = desc->tmp_alloc;

  world->std_alloc = std_alloc;
  world->tmp_alloc = tmp_alloc;

  // Create component stores
  {
    const uint32_t store_count = desc->component_count;
    ComponentStore *stores = NULL;
    if (store_count > 0) {
      stores = tb_alloc_nm_tp(std_alloc, store_count, ComponentStore);
      for (uint32_t i = 0; i < store_count; ++i) {
        const ComponentDescriptor *comp_desc = &desc->component_descs[i];
        create_component_store(&stores[i], comp_desc);
      }
    }

    world->component_store_count = store_count;
    world->component_stores = stores;
  }

  // Create systems
  {
    const uint32_t system_count = desc->system_count;
    world->system_count = system_count;

    System *systems = NULL;
    if (system_count > 0) {
      systems = tb_alloc_nm_tp(std_alloc, system_count, System);
      world->systems = systems;

      bool system_created = false;

      // Calculate the init order
      world->init_order =
          tb_alloc_nm_tp(std_alloc, desc->system_count, uint32_t);
      for (uint32_t i = 0; i < desc->system_count; ++i) {
        world->init_order[i] = find_system_idx_by_id(
            desc->system_descs, desc->system_count, desc->init_order[i]);
      }

      // Create in given init order
      for (uint32_t i = 0; i < desc->system_count; ++i) {
        uint32_t system_idx = world->init_order[i];
        const SystemDescriptor *sys_desc = &desc->system_descs[system_idx];

        system_created = create_system(world, &systems[system_idx], sys_desc);
      }

      // Calc tick order for later
      world->tick_order =
          tb_alloc_nm_tp(std_alloc, desc->system_count, uint32_t);
      for (uint32_t i = 0; i < desc->system_count; ++i) {
        world->tick_order[i] = find_system_idx_by_id(
            desc->system_descs, desc->system_count, desc->tick_order[i]);
      }
    }
  }

  return true;
}

bool tb_tick_world(World *world, float delta_seconds) {
  TracyCZoneN(world_tick_ctx, "World Tick", true);
  TracyCZoneColor(world_tick_ctx, TracyCategoryColorCore);

  Allocator tmp_alloc = world->tmp_alloc;

  {
    TracyCZoneN(system_tick_ctx, "Tick Systems", true);
    TracyCZoneColor(system_tick_ctx, TracyCategoryColorCore);

    // Tick in specified order
    for (uint32_t idx = 0; idx < world->system_count; idx++) {
      uint32_t system_idx = world->tick_order[idx];
      System *system = &world->systems[system_idx];

      // Gather and pack component columns for this system's input
      const uint32_t set_count = system->dep_count;

      SystemInput input = (SystemInput){
          .dep_set_count = set_count,
          .dep_sets = {{0}},
      };

      // Each dependency set can have a number of dependent columns
      for (uint32_t set_idx = 0; set_idx < set_count; ++set_idx) {
        // Gather the components that this dependency set requires
        const SystemComponentDependencies *dep = &system->deps[set_idx];
        SystemDependencySet *set = &input.dep_sets[set_idx];

        // Find the entities that have the required components
        uint32_t entity_count = 0;
        for (EntityId entity_id = 0; entity_id < world->entity_count;
             ++entity_id) {
          Entity entity = world->entities[entity_id];
          uint32_t matched_component_count = 0;
          for (uint32_t store_idx = 0; store_idx < world->component_store_count;
               ++store_idx) {
            const ComponentId store_id = world->component_stores[store_idx].id;
            for (uint32_t i = 0; i < dep->count; ++i) {
              if (store_id == dep->dependent_ids[i]) {
                if (entity & (1 << store_idx)) {
                  matched_component_count++;
                }
                break;
              }
            }
          }
          // Count if this entity has all required components
          if (matched_component_count == dep->count) {
            entity_count++;
          }
        }

        // Allocate collection of entities
        EntityId *entities = tb_alloc_nm_tp(tmp_alloc, entity_count, EntityId);
        entity_count = 0;
        for (EntityId entity_id = 0; entity_id < world->entity_count;
             ++entity_id) {
          Entity entity = world->entities[entity_id];
          uint32_t matched_component_count = 0;
          for (uint32_t store_idx = 0; store_idx < world->component_store_count;
               ++store_idx) {
            const ComponentId store_id = world->component_stores[store_idx].id;
            for (uint32_t i = 0; i < dep->count; ++i) {
              if (store_id == dep->dependent_ids[i]) {
                if (entity & (1 << store_idx)) {
                  matched_component_count++;
                }
                break;
              }
            }
          }
          // Count if this entity has all required components
          if (matched_component_count == dep->count) {
            entities[entity_count] = entity_id;
            entity_count++;
          }
        }
        set->entity_count = entity_count;
        set->entity_ids = entities;

        // Now we know how much we need to allocate for eached packed component
        // store
        set->column_count = dep->count;
        for (uint32_t col_id = 0; col_id < dep->count; ++col_id) {
          const ComponentId id = dep->dependent_ids[col_id];

          uint64_t components_size = 0;
          const ComponentStore *world_store = NULL;
          // Find world component store for this id and determine how much space
          // we need for the packed component store
          for (uint32_t comp_idx = 0; comp_idx < world->component_store_count;
               ++comp_idx) {
            const ComponentStore *store = &world->component_stores[comp_idx];
            if (store->id == id) {
              world_store = store;
              components_size = entity_count * store->size;
              break;
            }
          }
          if (components_size > 0) {
            uint8_t *components =
                tb_alloc_nm_tp(tmp_alloc, components_size, uint8_t);
            const uint64_t comp_size = world_store->size;

            // Copy from the world store based on entity index into the packed
            // store
            for (uint32_t entity_idx = 0; entity_idx < entity_count;
                 ++entity_idx) {
              EntityId entity_id = entities[entity_idx];

              const uint8_t *in_comp =
                  &world_store->components[entity_id * comp_size];
              uint8_t *out_comp = &components[entity_idx * comp_size];

              memcpy(out_comp, in_comp, comp_size);
            }

            set->columns[col_id] = (PackedComponentStore){
                .id = id,
                .components = components,
            };
          }
        }
      }

      SystemOutput output = (SystemOutput){0};

      system->tick(system->self, &input, &output, delta_seconds);

      // Write output back to world stores
      for (uint32_t set_idx = 0; set_idx < output.set_count; ++set_idx) {
        const SystemWriteSet *set = &output.write_sets[set_idx];

        // Get the world component store that matches this set's component id
        ComponentStore *comp_store = NULL;
        for (uint32_t store_idx = 0; store_idx < world->component_store_count;
             ++store_idx) {
          ComponentStore *store = &world->component_stores[store_idx];
          if (set->id == store->id) {
            comp_store = store;
            break;
          }
        }

        // Write out the components from the output set
        for (uint32_t entity_idx = 0; entity_idx < set->count; ++entity_idx) {
          EntityId entity_id = set->entities[entity_idx];

          const uint64_t comp_size = comp_store->size;

          const uint8_t *src = &set->components[entity_idx * comp_size];
          uint8_t *dst = &comp_store->components[entity_id * comp_size];

          memcpy(dst, src, comp_size);
        }
      }
    }
    TracyCZoneEnd(system_tick_ctx);
  }

  TracyCZoneEnd(world_tick_ctx);
  return true;
}

void tb_destroy_world(World *world) {
  if (world->component_store_count > 0) {
    for (uint32_t store_idx = 0; store_idx < world->component_store_count;
         ++store_idx) {
      ComponentStore *store = &world->component_stores[store_idx];
      for (EntityId entity_id = 0; entity_id < world->entity_count;
           ++entity_id) {
        Entity entity = world->entities[entity_id];
        uint8_t *comp = &store->components[(store->size * entity_id)];
        if (entity & (1 << store_idx)) {
          uint32_t system_dep_count = 0;
          System **system_deps = NULL;

          const ComponentDescriptor *comp_desc = &store->desc;

          if (comp_desc) {
            // Find system dependencies
            system_dep_count = comp_desc->system_dep_count;
            if (system_dep_count > 0) {
              system_deps =
                  tb_alloc_nm_tp(world->tmp_alloc, system_dep_count, System *);
              for (uint32_t dep_idx = 0; dep_idx < system_dep_count;
                   ++dep_idx) {
                for (uint32_t sys_idx = 0; sys_idx < world->system_count;
                     ++sys_idx) {
                  if (world->systems[sys_idx].id ==
                      comp_desc->system_deps[dep_idx]) {
                    system_deps[dep_idx] = &world->systems[sys_idx];
                    break;
                  }
                }
              }
            }
          }

          store->destroy(comp, system_dep_count, system_deps);
        }
      }
    }
    tb_free(world->std_alloc, world->component_stores);
  }

  if (world->system_count > 0) {
    // Shutdown in reverse init order
    for (int32_t i = world->system_count - 1; i >= 0; --i) {
      const uint32_t idx = world->init_order[i];
      System *system = &world->systems[idx];
      system->destroy(system->self);
      tb_free(world->std_alloc, system->self);
    }
    tb_free(world->std_alloc, world->systems);
  }

  *world = (World){0};
}

EntityId tb_world_add_entity(World *world, const EntityDescriptor *desc) {
  TracyCZoneNC(ctx, "Add Entity", TracyCategoryColorCore, true);
  // Determine if we need to grow the entity list
  const uint32_t new_entity_count = world->entity_count + 1;
  if (new_entity_count > world->max_entities) {
    world->max_entities = new_entity_count * 2;
    world->entities = tb_realloc_nm_tp(world->std_alloc, world->entities,
                                       world->max_entities, Entity);

    // Also resize *all* the component stores
    for (uint32_t store_idx = 0; store_idx < world->component_store_count;
         ++store_idx) {
      ComponentStore *store = &world->component_stores[store_idx];
      store->components =
          tb_realloc_nm_tp(world->std_alloc, store->components,
                           store->size * world->max_entities, uint8_t);
    }
  }
  EntityId entity_id = world->entity_count;
  Entity *entity = &world->entities[entity_id];
  *entity = 0; // Must initialize the entity
  world->entity_count++;

  for (uint32_t store_idx = 0; store_idx < world->component_store_count;
       ++store_idx) {
    ComponentStore *store = &world->component_stores[store_idx];

    // Determine if this component store will be referenced by this entity
    for (uint32_t comp_idx = 0; comp_idx < desc->component_count; ++comp_idx) {
      if (desc->component_ids[comp_idx] == store->id) {

        // Mark this store as being used by the entity
        (*entity) |= (1 << store_idx);

        store->count++;

        uint32_t system_dep_count = 0;
        System **system_deps = NULL;

        const ComponentDescriptor *comp_desc = &store->desc;

        if (comp_desc) {
          // Find system dependencies
          system_dep_count = comp_desc->system_dep_count;
          if (system_dep_count > 0) {
            system_deps =
                tb_alloc_nm_tp(world->tmp_alloc, system_dep_count, System *);
            for (uint32_t dep_idx = 0; dep_idx < system_dep_count; ++dep_idx) {
              for (uint32_t sys_idx = 0; sys_idx < world->system_count;
                   ++sys_idx) {
                if (world->systems[sys_idx].id ==
                    comp_desc->system_deps[dep_idx]) {
                  system_deps[dep_idx] = &world->systems[sys_idx];
                  break;
                }
              }
            }
          }
        }

        // Create a component in the store at this entity index
        uint8_t *comp_head = &store->components[entity_id * store->size];
        if (!store->create(comp_head, desc->component_descriptors[comp_idx],
                           system_dep_count, system_deps)) {
          TracyCZoneEnd(ctx);
          return (EntityId)-1;
        }

        break;
      }
    }
  }

  TracyCZoneEnd(ctx);
  return entity_id;
}

const PackedComponentStore *tb_get_column_check_id(const SystemInput *input,
                                                   uint32_t set, uint32_t index,
                                                   ComponentId id) {
  TracyCZoneNC(ctx, "get column check id", TracyCategoryColorCore, true);
  if (set >= input->dep_set_count) {
    TracyCZoneEnd(ctx);
    return NULL;
  }

  const SystemDependencySet *dep = &input->dep_sets[set];
  if (index >= dep->column_count) {
    TracyCZoneEnd(ctx);
    return NULL;
  }

  // Make sure it's the id the caller wanted
  const PackedComponentStore *store = &dep->columns[index];
  if (store->id == id) {
    TracyCZoneEnd(ctx);
    return store;
  }

  TracyCZoneEnd(ctx);
  return NULL;
}

uint32_t tb_get_column_component_count(const SystemInput *input, uint32_t set) {
  const SystemDependencySet *dep = &input->dep_sets[set];
  return dep->entity_count;
}

EntityId *tb_get_column_entity_ids(const SystemInput *input, uint32_t set) {
  const SystemDependencySet *dep = &input->dep_sets[set];
  return dep->entity_ids;
}

System *tb_find_system_by_id(System *systems, uint32_t system_count,
                             SystemId id) {
  TracyCZoneNC(ctx, "Find System By Id", TracyCategoryColorCore, true);
  for (uint32_t i = 0; i < system_count; ++i) {
    System *system = &systems[i];
    if (system->id == id) {
      TracyCZoneEnd(ctx);
      return system;
    }
  }
  TracyCZoneEnd(ctx);
  return NULL;
}

System *tb_find_system_dep_by_id(System *const *systems, uint32_t system_count,
                                 SystemId id) {
  TracyCZoneNC(ctx, "Find System By Dependency Id", TracyCategoryColorCore,
               true);
  for (uint32_t i = 0; i < system_count; ++i) {
    System *system = systems[i];
    if (system->id == id) {
      TracyCZoneEnd(ctx);
      return system;
    }
  }
  TracyCZoneEnd(ctx);
  return NULL;
}
void *tb_find_system_dep_self_by_id(System *const *systems,
                                    uint32_t system_count, SystemId id) {
  System *sys = tb_find_system_dep_by_id(systems, system_count, id);
  if (sys) {
    return sys->self;
  }
  return NULL;
}
