#include "orbitsystem.hpp"

#include <tracy/Tracy.hpp>

OrbitSystem::OrbitSystem() {}
OrbitSystem::~OrbitSystem() {}

SystemInputQuerySet OrbitSystem::get_input_queries() const {
  ZoneScopedN("OrbitSystem::get_input_queries");
  std::array<ComponentId, 2> query = {
      TransformComponentId,
      OrbitComponentId,
  };

  std::array<SystemInputQuery, 1> queries = {SystemInputQuery{
      .components = query,
  }};

  SystemInputQuerySet query_sets = {
      .sets = queries,
  };

  return query_sets;
}

void OrbitSystem::tick(const SystemInput &input, SystemOutput &output,
                       float delta_seconds) {
  ZoneScopedN("OrbitSystem::tick");
}