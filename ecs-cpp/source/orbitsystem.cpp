#include "orbitsystem.hpp"

#include "simd.hpp"

#include <SDL2/SDL_log.h>
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

  const SystemInputSet &set = input[0];
  const size_t orbiter_count = set.entities.size();

  auto &trans_comps = make_out_copy<TransformComponent>(set.stores[0]);
  auto &orbit_comps = make_out_copy<OrbitComponent>(set.stores[1]);

  auto *trans_writes =
      tmp_alloc->alloc_num<SystemWrite<TransformComponent>>(orbiter_count);
  auto *orbit_writes =
      tmp_alloc->alloc_num<SystemWrite<OrbitComponent>>(orbiter_count);

  for (size_t i = 0; i < orbiter_count; ++i) {
    const EntityId entity = set.entities[i];
    auto &trans_comp = trans_comps.components[i];
    auto &orbit_comp = orbit_comps.components[i];

    float angle = orbit_comp.speed * delta_seconds;

    Quaternion rot = angle_axis_to_quat(f3tof4(orbit_comp.axis, angle));
    rot = mulq(orbit_comp.rotation, rot);
    orbit_comp.rotation = rot;
    float3 dir = (float3){1, 0.0f, 0.0f};
    dir = normf3(qrotf3(rot, dir));
    float3 point = dir * orbit_comp.distance;

    Transform &transform = trans_comp.transform;
    transform.position = orbit_comp.center + point;

    float3 pos = transform.position;
    {
      ZoneScopedN("Logging");
      SDL_Log("Orbiting to: (%f, %f, %f)", pos[0], pos[1], pos[2]);
    }

    trans_writes[i] = {entity, &trans_comp};
    orbit_writes[i] = {entity, &orbit_comp};
  }

  auto *trans_write_set =
      tmp_alloc
          ->alloc<SystemWriteSet<TransformComponent, TransformComponentId>>();
  new (trans_write_set)(
      SystemWriteSet<TransformComponent, TransformComponentId>);
  trans_write_set->writes =
      std::span<SystemWrite<TransformComponent>>(trans_writes, orbiter_count);
  auto *orbit_write_set =
      tmp_alloc->alloc<SystemWriteSet<OrbitComponent, OrbitComponentId>>();
  new (orbit_write_set)(SystemWriteSet<OrbitComponent, OrbitComponentId>);
  orbit_write_set->writes =
      std::span<SystemWrite<OrbitComponent>>(orbit_writes, orbiter_count);

  const size_t write_set_count = 2;
  auto *writes =
      tmp_alloc->alloc_num<const SystemWriteSetBase *>(write_set_count);
  writes[0] = static_cast<const SystemWriteSetBase *>(trans_write_set);
  writes[1] = static_cast<const SystemWriteSetBase *>(orbit_write_set);

  *output = SystemOutput(writes, write_set_count);
  return *output;
}