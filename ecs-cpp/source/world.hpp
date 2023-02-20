#pragma once

#include "arenaalloc.hpp"
#include "component.hpp"
#include "stdalloc.hpp"

#include <tracy/Tracy.hpp>
#include <tuple>
#include <unordered_map>

class System;

using EntityId = uint32_t;
using Entity = uint32_t;

template <typename... SystemTypes>
struct SystemStorage : public std::tuple<SystemTypes...> {
  SystemStorage() : std::tuple<SystemTypes...>() {}
};

template <typename... ComponentTypes>
struct ComponentStorage : public std::tuple<std::vector<ComponentTypes>...> {
  ComponentStorage() : std::tuple<std::vector<ComponentTypes>...>() {}
};

template <typename Systems, typename Components> class World {

public:
  World(StdAllocator *std_alloc, ArenaAllocator *tmp_alloc) {
    std_alloc = std_alloc;
    tmp_alloc = tmp_alloc;

    // Create a mapping between index in the Components tuple and the
    // ids that the runtime uses to identify components
    register_components(components);
  }
  ~World() {}

  bool tick(float delta_seconds) {
    ZoneScopedN("World Tick");

    tick(systems, delta_seconds);

    return true;
  }

  EntityId add_entity(const std::vector<const ComponentBase *> &descs) {
    ZoneScopedN("Add Entity");

    add_entity_components(components);
    EntityId id = static_cast<EntityId>(entities.size());

    // Determine which bits should be set on the entity
    entities.push_back(create_entity(descs));
    return id;
  }

private:
  using Entities = std::vector<Entity>;
  using ComponentRuntimeMap = std::unordered_map<ComponentId, size_t>;

  // Iterating a tuple at compile tile
  template <std::size_t I = 0, typename... Tp>
  inline typename std::enable_if<I == sizeof...(Tp), void>::type
  register_components(std::tuple<Tp...> &t) {}

  template <std::size_t I = 0, typename... Tp>
      inline typename std::enable_if <
      I<sizeof...(Tp), void>::type register_components(std::tuple<Tp...> &t) {
    using vector_type = std::tuple_element_t<I, std::tuple<Tp...>>;
    typename vector_type::value_type comp = {};
    ComponentId id = comp.get_id();
    comp_map[id] = I;
    register_components<I + 1, Tp...>(t);
  }

  template <std::size_t I = 0, typename... Tp>
  inline typename std::enable_if<I == sizeof...(Tp), void>::type
  tick(std::tuple<Tp...> &t, float delta_seconds) {}

  template <std::size_t I = 0, typename... Tp>
      inline typename std::enable_if <
      I<sizeof...(Tp), void>::type tick(std::tuple<Tp...> &t,
                                        float delta_seconds) {
    SystemOutput out = {};
    std::get<I>(t).tick({}, out, delta_seconds);
    tick<I + 1, Tp...>(t, delta_seconds);
  }

  template <std::size_t I = 0, typename... Tp>
  inline typename std::enable_if<I == sizeof...(Tp), void>::type
  add_entity_components(std::tuple<Tp...> &t) {}

  template <std::size_t I = 0, typename... Tp>
      inline typename std::enable_if <
      I<sizeof...(Tp), void>::type add_entity_components(std::tuple<Tp...> &t) {
    std::get<I>(t).push_back({});
    add_entity_components<I + 1, Tp...>(t);
  }

  Entity create_entity(const std::vector<const ComponentBase *> &descs) {
    Entity entity = 0;
    for (auto desc : descs) {
      ComponentId id = desc->get_id();
      if (comp_map.contains(id)) {
        size_t comp_idx = comp_map[id];
        entity |= 1 << comp_idx;
      }
    }
    return entity;
  };

  Systems systems;
  Components components;
  Entities entities;
  ComponentRuntimeMap comp_map;

  StdAllocator *std_alloc;
  ArenaAllocator *tmp_alloc;
};
