#include "orbitsystem.hpp"

#include <tracy/Tracy.hpp>

OrbitSystem::OrbitSystem() {}
OrbitSystem::~OrbitSystem() {}

void OrbitSystem::tick(const SystemInput &input, SystemOutput &output,
                       float delta_seconds) {
  ZoneScopedN("OrbitSystem");
}