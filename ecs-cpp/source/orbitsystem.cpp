#include "orbitsystem.hpp"

#include <tracy/Tracy.hpp>

OrbitSystem::OrbitSystem(uint32_t id) : System(id) {}
OrbitSystem::~OrbitSystem() {}

void OrbitSystem::tick(const SystemInput &input, SystemOutput &output,
                       float delta_seconds) {
  ZoneScopedN("OrbitSystem");
  _sleep(50);
}