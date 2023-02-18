#pragma once

#include "simd.h"
#include <stdint.h>

#define TransformComponentId 0xDEAD0000

typedef uint32_t EntityId;
typedef struct World World;
typedef struct ComponentStore ComponentStore;
typedef struct ComponentDescriptor ComponentDescriptor;

typedef struct TransformComponentDescriptor {
  World *world;
  EntityId parent;
  Transform transform;
} TransformComponentDescriptor;

typedef struct TransformComponent {
  float4x4 world_matrix;
  ComponentStore *transform_store;
  Transform transform;
  EntityId parent;
  uint32_t child_count;
  EntityId *children;
} TransformComponent;

void tb_transform_component_descriptor(ComponentDescriptor *desc);

TransformComponent *tb_transform_get_parent(TransformComponent *self);

float4x4 tb_transform_get_world_matrix(TransformComponent *self);
