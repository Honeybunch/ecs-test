#pragma once

#include "stdalloc.hpp"

#include <span>
#include <stdint.h>

using ComponentId = uint32_t;
using SystemId = uint32_t;

struct SystemInput {};

struct SystemOutput {};

struct SystemInputQuery : public std::span<ComponentId> {};

struct SystemInputQuerySet : public std::span<SystemInputQuery> {};

class System {
public:
  virtual SystemInputQuerySet get_input_queries() const = 0;

  virtual void tick(const SystemInput &input, SystemOutput &output,
                    float delta_seconds) = 0;
};
