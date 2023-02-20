#pragma once

#include "component.hpp"
#include "simd.hpp"

static constexpr ComponentId TransformComponentId = 0xDEAD0001;

struct TransformComponentDescriptor
    : public ComponentDescriptor<TransformComponentId> {
  Transform transform;
};

class TransformComponent : public Component<TransformComponentId> {
public:
  TransformComponent() {}
  TransformComponent(const TransformComponentDescriptor &desc)
      : transform(desc.transform) {}
  ~TransformComponent() {}

  Transform &get_transform();
  const Transform &get_transform() const;

private:
  Transform transform;
};
