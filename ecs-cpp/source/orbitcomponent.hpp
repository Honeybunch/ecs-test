#pragma once

#include "component.hpp"
#include "simd.hpp"

#include <new>

static constexpr ComponentId OrbitComponentId = 0xDEAD0000;

struct OrbitComponentDescriptor;

class OrbitComponent : public Component {
public:
  OrbitComponent(const OrbitComponentDescriptor &desc);
  ~OrbitComponent();

private:
  float3 center;
  float distance;
  float3 axis;
  float speed;
  Quaternion rotation;
};

struct OrbitComponentDescriptor : public ComponentDescriptor {
  OrbitComponentDescriptor() { id = OrbitComponentId; }
  float3 center;
  float distance;
  float3 axis;
  float speed;
};

using OrbitComponentStore =
    ComponentStore<OrbitComponent, OrbitComponentDescriptor>;