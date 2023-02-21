#pragma once

#include "stdalloc.hpp"

#include <stdint.h>

using SystemId = uint32_t;

struct SystemInput {};

struct SystemOutput {};

class System {
public:
  System() {}
  ~System() {}
  virtual void tick(const SystemInput &input, SystemOutput &output,
                    float delta_seconds) = 0;
};
