#include "orbitcomponent.hpp"

OrbitComponent::OrbitComponent() : rotation(Quaternion{0, 0, 0, 1}) {}
OrbitComponent::OrbitComponent(const OrbitComponentDescriptor &desc)
    : center(desc.center), distance(desc.distance), axis(desc.axis),
      speed(desc.speed), rotation(Quaternion{0, 0, 0, 1}) {}
OrbitComponent::~OrbitComponent() {}