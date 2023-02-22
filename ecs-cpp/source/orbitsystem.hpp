#pragma once

#include "system.hpp"

#include "orbitcomponent.hpp"
#include "transformcomponent.hpp"

#include <array>

using ComponentId = uint32_t;

class OrbitSystemDescriptor;

static constexpr SystemId OrbitSystemId = 0xDEADFF00;

class OrbitSystem : public System {
public:
  OrbitSystem(StdAllocator &std_alloc, ArenaAllocator &tmp_alloc);
  ~OrbitSystem();

  SystemInputQuerySet get_input_queries() const override;

  const SystemOutput &tick(const SystemInput &input,
                           float delta_seconds) override;
};
