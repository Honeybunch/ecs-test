#include "orbitsystem.hpp"

#include <tracy/Tracy.hpp>

OrbitSystem::OrbitSystem(StdAllocator &std_alloc, ArenaAllocator &tmp_alloc)
    : System(std_alloc, tmp_alloc) {}
OrbitSystem::~OrbitSystem() {}

SystemInputQuerySet OrbitSystem::get_input_queries() const {
  ZoneScopedN("OrbitSystem::get_input_queries");

  static std::array<ComponentId, 2> query = {
      TransformComponentId,
      OrbitComponentId,
  };
  static std::array<SystemInputQuery, 1> queries = {SystemInputQuery(query)};

  return SystemInputQuerySet(queries);
}

const SystemOutput &OrbitSystem::tick(const SystemInput &input,
                                      float delta_seconds) {
  ZoneScopedN("OrbitSystem::tick");
  auto *output = System::tmp_alloc->alloc<SystemOutput>();

  return *output;
}