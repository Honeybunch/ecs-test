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
  World(const WorldDescriptor &desc);
  ~World();

  template <SystemId id, typename T> void register_system() {
    systems[id] = static_cast<System *>(new (std_alloc->alloc<T>())
                                            T(*std_alloc, *tmp_alloc));
  }

  template <ComponentId id, typename T> void register_component() {
    const size_t store_count = stores.size();
    stores[id] = static_cast<ComponentStoreBase *>(
        new (std_alloc->alloc<T>()) T(*std_alloc, store_count));
  }

  bool tick(float delta_seconds);
  EntityId add_entity(const std::span<const ComponentDescriptorBase *> &descs);
  const SystemInput &filter_system_input(System *system);
  void write_system_output(const SystemOutput &out);

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
