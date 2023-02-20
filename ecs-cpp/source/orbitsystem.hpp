#pragma once

#include "system.hpp"

class OrbitSystem : public System {
public:
  OrbitSystem();
  ~OrbitSystem();

  void tick(const SystemInput &input, SystemOutput &output,
            float delta_seconds) override;
};