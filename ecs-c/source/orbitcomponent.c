#include "orbitcomponent.h"

#include "world.h"

bool create_orbit_component(OrbitComponent *comp,
                            const OrbitComponentDescriptor *desc,
                            uint32_t system_dep_count,
                            System *const *system_deps) {
  (void)system_dep_count;
  (void)system_deps;
  *comp = (OrbitComponent){
      .center = desc->center,
      .distance = desc->distance,
      .axis = desc->axis,
      .speed = desc->speed,
      .rotation = {0, 0, 0, 1},
  };
  return true;
}

void destroy_orbit_component(OrbitComponent *comp, uint32_t system_dep_count,
                             System *const *system_deps) {
  (void)system_dep_count;
  (void)system_deps;
  *comp = (OrbitComponent){0};
}

TB_DEFINE_COMPONENT(orbit, OrbitComponent, OrbitComponentDescriptor)

void tb_orbit_component_descriptor(ComponentDescriptor *desc) {
  *desc = (ComponentDescriptor){
      .name = "Orbit",
      .size = sizeof(OrbitComponent),
      .id = OrbitComponentId,
      .create = tb_create_orbit_component,
      .destroy = tb_destroy_orbit_component,
  };
}
