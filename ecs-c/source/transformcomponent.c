#include "transformcomponent.h"

#include "profiling.h"
#include "world.h"

bool create_transform_component(TransformComponent *comp,
                                const TransformComponentDescriptor *desc,
                                uint32_t system_dep_count,
                                System *const *system_deps) {
  (void)system_dep_count;
  (void)system_deps;

  *comp = (TransformComponent){
      .transform = desc->transform,
  };

  return true;
}

void destroy_transform_component(TransformComponent *comp,
                                 uint32_t system_dep_count,
                                 System *const *system_deps) {
  (void)system_dep_count;
  (void)system_deps;
  // Setting scale to 0 to implicitly zero out the entire object
  // while avoiding nonsense warnings from the compiler in IDEs
  *comp = (TransformComponent){
      .transform =
          {
              .scale = (float3){0},
              .rotation = (Quaternion){0, 0, 0, 1},
          },
  };
}

TB_DEFINE_COMPONENT(transform, TransformComponent, TransformComponentDescriptor)

void tb_transform_component_descriptor(ComponentDescriptor *desc) {
  *desc = (ComponentDescriptor){
      .name = "Transform",
      .size = sizeof(TransformComponent),
      .id = TransformComponentId,
      .create = tb_create_transform_component,
      .destroy = tb_destroy_transform_component,
  };
}
