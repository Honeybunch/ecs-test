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
    SystemInput input = {};
    filter_system_input(system, input);

    // Tick the system to retrieve output
    SystemOutput out = {};
    system->tick(input, out, delta_seconds);

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

void World::filter_system_input(System *system, SystemInput &input) {
  ZoneScopedN("World::filter_system_input");
  const SystemInputQuerySet &input_queries = system->get_input_queries();
  input = {};
}

void World::write_system_output(const SystemOutput &out) {
  ZoneScopedN("World::write_system_output");
}