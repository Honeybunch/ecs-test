#pragma once

#include "system.hpp"

#include <new>

class OrbitSystemDescriptor;

static constexpr SystemId OrbitSystemId = 0xDEADFF00;

class OrbitSystem : public System {
public:
  OrbitSystem();
  ~OrbitSystem();

  void tick(const SystemInput &input, SystemOutput &output,
            float delta_seconds) override;
};
