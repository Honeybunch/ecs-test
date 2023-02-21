#pragma once

#include "arenaalloc.hpp"
#include "stdalloc.hpp"

#include <span>

class World;

class Application {
public:
  Application(StdAllocator &std_alloc, ArenaAllocator &tmp_alloc,
              std::span<World *> worlds);
};
