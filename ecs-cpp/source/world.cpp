#include "world.hpp"

World::World(const WorldDescriptor &desc) {
  std_alloc = desc.std_alloc;
  tmp_alloc = desc.tmp_alloc;
}

World::~World() {
  for (const auto &kvp : systems) {
    std_alloc->free(kvp.second);
  }
  for (const auto &kvp : stores) {
    std_alloc->free(kvp.second);
  }
}

bool World::tick(float delta_seconds) {
  ZoneScopedN("World::tick");

  for (const auto &kvp : systems) {
    System *system = kvp.second;

    // Gather packed input for the system
    const SystemInput &input = filter_system_input(system);

    // Tick the system to retrieve output
    const SystemOutput &out = system->tick(input, delta_seconds);

    // Write system output back to the world
    write_system_output(out);
  }

  return true;
}

EntityId
World::add_entity(const std::span<const ComponentDescriptorBase *> &descs) {
  ZoneScopedN("World::add_entity");

  EntityId id = static_cast<EntityId>(entities.size());
  Entity entity = 0;
  // Add a component to each column
  uint32_t store_idx = 0;
  for (const auto &kvp : stores) {
    kvp.second->alloc_component();
    for (const auto &desc : descs) {
      // If component matches one of the given descriptors, mark it on the
      // entity and initialize it
      if (desc->get_id() == kvp.first) {
        entity |= 1 << store_idx;
        kvp.second->init_component(id, desc);
        break;
      }
    }
    store_idx++;
  }

  entities.push_back(entity);
  return id;
}

const SystemInput &World::filter_system_input(System *system) {
  ZoneScopedN("World::filter_system_input");
  const SystemInputQuerySet &input_queries = system->get_input_queries();

  // Each input query gets its own space in the input structure
  const size_t query_count = input_queries.size();
  SystemInputSet *sets = tmp_alloc->alloc_num<SystemInputSet>(query_count);

  for (size_t query_idx = 0; query_idx < query_count; ++query_idx) {
    const auto &query = input_queries[query_idx];
    // Figure out which entities match this filter
    // Worst case all entities do
    EntityId *entity_ids = tmp_alloc->alloc_num<EntityId>(entities.size());
    size_t entity_count = 0;

    for (const auto &entity : entities) {
      size_t matching_comp_count = 0;
      for (const auto &comp_id : query) {
        // Determine if entity has the given component
        const size_t store_idx = stores[comp_id]->get_store_idx();
        if (entity & (1 << store_idx)) {
          matching_comp_count++;
        }
      }

      if (matching_comp_count == query.size()) {
        entity_ids[entity_count++] = entity;
      }
    }

    // No entities have the required components? Give up on this query
    if (entity_count == 0) {
      continue;
    }

    std::span<EntityId> input_entities =
        std::span<EntityId>(entity_ids, entity_count);

    // One store per component in the query
    auto *packed_stores =
        tmp_alloc->alloc_num<const PackedComponentStoreBase *>(query.size());
    {
      size_t i = 0;
      for (ComponentId id : query) {
        packed_stores[i] =
            stores[id]->pack_components(*tmp_alloc, input_entities);
        i++;
      }
    }

    // Allocate space in this set for each component and entity
    SystemInputSet &set = sets[query_idx];
    set.entities = input_entities;
    set.stores = std::span<const PackedComponentStoreBase *>(packed_stores,
                                                             query.size());
  }

  auto *input = tmp_alloc->alloc<SystemInput>();
  *input = SystemInput(sets, query_count);
  return *input;
}

void World::write_system_output(const SystemOutput &out) {
  ZoneScopedN("World::write_system_output");
}