#pragma once

#include "component.hpp"
#include "simd.hpp"

static constexpr ComponentId OrbitComponentId = 0xDEAD0000;

struct OrbitComponentDescriptor;

class OrbitComponent : public Component {
public:
  OrbitComponent(const OrbitComponentDescriptor &desc);
  ~OrbitComponent();

  float3 center;
  float distance;
  float3 axis;
  float speed;
  Quaternion rotation;
};

struct OrbitComponentDescriptor : public ComponentDescriptor<OrbitComponentId> {
  float3 center;
  float distance;
  float3 axis;
  float speed;
};

using OrbitComponentStore =
    ComponentStore<OrbitComponent, OrbitComponentDescriptor>;
