#pragma once

#include "component.hpp"
#include "simd.hpp"

class TransformComponent : public Component {
public:
  TransformComponent() {}
  ~TransformComponent() {}

  Transform &get_transform();
  const Transform &get_transform() const;

private:
  Transform transform;
};
