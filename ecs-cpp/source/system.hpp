#pragma once

#include <stdint.h>

struct SystemInput {};

struct SystemOutput {};

class System {
public:
  System(uint32_t id) : id(id) {}
  ~System() {}

  uint32_t get_id() const { return id; }

  virtual void tick(const SystemInput &input, SystemOutput &output,
                    float delta_seconds) = 0;

private:
  uint32_t id;
};