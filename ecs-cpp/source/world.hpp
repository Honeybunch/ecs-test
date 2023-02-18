#pragma once

#include "arenaalloc.hpp"
#include "stdalloc.hpp"

#include <tracy/Tracy.hpp>

class System;

class SystemsBase {
public:
  const std::vector<System *> &get_systems() const { return systems; };

protected:
  std::vector<System *> systems;
};

template <typename Systems> class World {
  static_assert(std::is_base_of<SystemsBase, Systems>::value,
                "Systems must derive from SystemsBase");

public:
  World(StdAllocator *std_alloc, ArenaAllocator *tmp_alloc) {
    std_alloc = std_alloc;
    tmp_alloc = tmp_alloc;
  }
  ~World() {}

  bool tick(float delta_seconds) {
    ZoneScopedN("World Tick");
    _sleep(50);
    for (System *system : systems.get_systems()) {
      SystemOutput output = {};
      system->tick({}, output, delta_seconds);
    }
    return true;
  }

private:
  Systems systems;
  StdAllocator *std_alloc;
  ArenaAllocator *tmp_alloc;

  template <typename Sys> System &get_system(uint32_t idx) {
    return static_cast<Sys>(systems[idx]);
  }
};
