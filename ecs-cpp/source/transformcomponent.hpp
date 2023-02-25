#pragma once

#include "component.hpp"
#include "simd.hpp"

#include <new>

static constexpr ComponentId TransformComponentId = 0xDEAD0001;

struct TransformComponentDescriptor;

class TransformComponent : public Component {
public:
  TransformComponent();
  TransformComponent(const TransformComponentDescriptor &desc);
  ~TransformComponent();

  Transform transform = {};
};

struct TransformComponentDescriptor
    : public ComponentDescriptor<TransformComponentId> {
  Transform transform;
};

using TransformComponentStore =
    ComponentStore<TransformComponent, TransformComponentDescriptor,
                   TransformComponentId>;
