#pragma once

#include "simd.h"
#include <stdint.h>

#define OrbitComponentId 0xDEAD0001

typedef struct ComponentDescriptor ComponentDescriptor;

typedef struct OrbitComponentDescriptor {
  float3 center;
  float distance;
  float3 axis;
  float speed;
} OrbitComponentDescriptor;

typedef struct OrbitComponent {
  float3 center;
  float distance;
  float3 axis;
  float speed;
  Quaternion rotation;
} OrbitComponent;

void tb_orbit_component_descriptor(ComponentDescriptor *desc);
