#pragma once

#include "component.hpp"
#include "simd.hpp"

static constexpr ComponentId OrbitComponentId = 0xDEAD0000;

struct OrbitComponentDescriptor : public ComponentDescriptor<OrbitComponentId> {
  float3 center;
  float distance;
  float3 axis;
  float speed;
};

class OrbitComponent : public Component<OrbitComponentId> {
public:
  OrbitComponent(float3 center, float distance, float3 axis, float speed)
      : center(center), distance(distance), axis(axis), speed(speed),
        rotation(Quaternion{0, 0, 0, 1}) {}
  OrbitComponent(const OrbitComponentDescriptor &desc)
      : center(desc.center), distance(desc.distance), axis(desc.axis),
        speed(desc.speed), rotation(Quaternion{0, 0, 0, 1}) {}
  OrbitComponent() {}
  ~OrbitComponent() {}

private:
  float3 center;
  float distance;
  float3 axis;
  float speed;
  Quaternion rotation;
};