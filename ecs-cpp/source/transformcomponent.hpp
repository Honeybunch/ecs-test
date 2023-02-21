#pragma once

#include "component.hpp"
#include "simd.hpp"

#include <new>

static constexpr ComponentId TransformComponentId = 0xDEAD0001;

struct TransformComponentDescriptor;

class TransformComponent : public Component {
public:
  TransformComponent(const TransformComponentDescriptor &desc);
  ~TransformComponent();

  Transform &get_transform();
  const Transform &get_transform() const;

private:
  Transform transform;
};

struct TransformComponentDescriptor : public ComponentDescriptor {
  TransformComponentDescriptor() { id = TransformComponentId; }
  Transform transform;
};

using TransformComponentStore =
    ComponentStore<TransformComponent, TransformComponentDescriptor>;
