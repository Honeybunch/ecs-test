#include "orbitsystem.h"

#include "orbitcomponent.h"
#include "profiling.h"
#include "transformcomponent.h"
#include "world.h"

#include <SDL2/SDL_log.h>

TB_DEFINE_SYSTEM(orbit, OrbitSystem, OrbitSystemDescriptor);

bool create_orbit_system(OrbitSystem *self, const OrbitSystemDescriptor *desc,
                         uint32_t system_dep_count,
                         System *const *system_deps) {
  (void)system_dep_count;
  (void)system_deps;
  *self = (OrbitSystem){
      .tmp_alloc = desc->tmp_alloc,
  };
  return true;
}

void destroy_orbit_system(OrbitSystem *self) { *self = (OrbitSystem){0}; }

void tick_orbit_system(OrbitSystem *self, const SystemInput *input,
                       SystemOutput *output, float delta_seconds) {
  TracyCZoneNC(ctx, "Orbit System", TracyCategoryColorGame, true);
  EntityId *out_ids = tb_get_column_entity_ids(input, 0);
  uint32_t orbiter_count = tb_get_column_component_count(input, 0);
  const PackedComponentStore *transform_store =
      tb_get_column_check_id(input, 0, 0, TransformComponentId);
  const PackedComponentStore *orbit_store =
      tb_get_column_check_id(input, 0, 1, OrbitComponentId);

  // We're going to write out modified transforms so make a copy
  tb_make_out_copy(out_trans_comps, self->tmp_alloc, transform_store,
                   orbiter_count, TransformComponent);
  tb_make_out_copy(out_orbit_comps, self->tmp_alloc, orbit_store, orbiter_count,
                   OrbitComponent);

  for (uint32_t i = 0; i < orbiter_count; ++i) {
    TransformComponent *trans_comp = &out_trans_comps[i];
    OrbitComponent *orbit_comp = &out_orbit_comps[i];
    (*trans_comp) = *tb_get_component(transform_store, i, TransformComponent);
    (*orbit_comp) = *tb_get_component(orbit_store, i, OrbitComponent);

    float angle = orbit_comp->speed * delta_seconds;

    Quaternion rot = angle_axis_to_quat(f3tof4(orbit_comp->axis, angle));
    rot = mulq(orbit_comp->rotation, rot);
    orbit_comp->rotation = rot;
    float3 dir = (float3){1, 0.0f, 0.0f};
    dir = normf3(qrotf3(rot, dir));
    float3 point = dir * orbit_comp->distance;

    Transform *transform = &trans_comp->transform;
    transform->position = orbit_comp->center + point;

    float3 pos = transform->position;
    TracyCZoneNC(log_ctx, "Logging", TracyCategoryColorGame, true);
    SDL_Log("Orbiting to: (%f, %f, %f)", pos[0], pos[1], pos[2]);
    TracyCZoneEnd(log_ctx);
  }

  *output = (SystemOutput){
      .set_count = 2,
      .write_sets[0] =
          {
              .components = (uint8_t *)out_trans_comps,
              .count = orbiter_count,
              .entities = out_ids,
              .id = TransformComponentId,
          },
      .write_sets[1] =
          {
              .components = (uint8_t *)out_orbit_comps,
              .count = orbiter_count,
              .entities = out_ids,
              .id = OrbitComponentId,
          },
  };
  TracyCZoneEnd(ctx);
}

void tb_orbit_system_descriptor(SystemDescriptor *desc,
                                const OrbitSystemDescriptor *sys_desc) {
  *desc = (SystemDescriptor){
      .name = "Orbit",
      .size = sizeof(OrbitSystem),
      .id = OrbitSystemId,
      .desc = sys_desc,
      .dep_count = 1,
      .deps[0] =
          (SystemComponentDependencies){
              .count = 2,
              .dependent_ids = {TransformComponentId, OrbitComponentId},
          },
      .create = tb_create_orbit_system,
      .destroy = tb_destroy_orbit_system,
      .tick = tb_tick_orbit_system,
  };
}