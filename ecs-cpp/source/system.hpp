#pragma once

#include <stdint.h>

struct SystemInput {};

struct SystemOutput {};

class System {
public:
  System() {}
  ~System() {}
  virtual void tick(const SystemInput &input, SystemOutput &output,
                    float delta_seconds) = 0;
};