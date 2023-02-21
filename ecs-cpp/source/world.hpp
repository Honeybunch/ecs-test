#pragma once

#include "arenaalloc.hpp"
#include "component.hpp"
#include "stdalloc.hpp"
#include "system.hpp"

#include <tracy/Tracy.hpp>

#include <new>
#include <span>
#include <tuple>
#include <unordered_map>

class System;

using EntityId = size_t;
using Entity = uint32_t;

struct WorldDescriptor {
  StdAllocator *std_alloc;
  ArenaAllocator *tmp_alloc;
};

class World {

public:
  World(const WorldDescriptor &desc) {
    std_alloc = desc.std_alloc;
    tmp_alloc = desc.tmp_alloc;
  }
  ~World() {
    for (const auto &kvp : systems) {
      std_alloc->free(kvp.second);
    }
    for (const auto &kvp : stores) {
      std_alloc->free(kvp.second);
    }
  }

  template <SystemId id, typename T> void register_system() {
    systems[id] = static_cast<System *>(new (std_alloc->alloc<T>()) T());
  }

  template <ComponentId id, typename T> void register_component() {
    stores[id] = static_cast<ComponentStoreBase *>(new (std_alloc->alloc<T>())
                                                       T(*std_alloc));
  }

  bool tick(float delta_seconds) {
    ZoneScopedN("World Tick");

    for (const auto &kvp : systems) {
      SystemOutput out = {};
      kvp.second->tick({}, out, delta_seconds);
    }

    return true;
  }

  EntityId add_entity(const std::span<const ComponentDescriptor *> &descs) {
    ZoneScopedN("Add Entity");

    EntityId id = static_cast<EntityId>(entities.size());
    Entity entity = 0;
    // Add a component to each column
    uint32_t store_idx = 0;
    for (const auto &kvp : stores) {
      kvp.second->alloc_component();
      for (const auto &desc : descs) {
        // If component matches one of the given descriptors, mark it on the
        // entity and initialize it
        if (desc->id == kvp.first) {
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

private:
  using Entities = std::vector<Entity>;
  using Systems = std::unordered_map<SystemId, System *>;
  using ComponentStores = std::unordered_map<ComponentId, ComponentStoreBase *>;

  Entities entities;
  Systems systems;
  ComponentStores stores;

  StdAllocator *std_alloc;
  ArenaAllocator *tmp_alloc;
};
