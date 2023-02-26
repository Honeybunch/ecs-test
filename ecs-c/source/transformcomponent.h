#pragma once

#include "simd.h"
#include <stdint.h>

#define TransformComponentId 0xDEAD0000

typedef struct ComponentStore ComponentStore;
typedef struct ComponentDescriptor ComponentDescriptor;

typedef struct TransformComponentDescriptor {
  Transform transform;
} TransformComponentDescriptor;

typedef struct TransformComponent {
  Transform transform;
} TransformComponent;

void tb_transform_component_descriptor(ComponentDescriptor *desc);

TransformComponent *tb_transform_get_parent(TransformComponent *self);