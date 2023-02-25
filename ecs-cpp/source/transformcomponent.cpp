#include "transformcomponent.hpp"

TransformComponent::TransformComponent() {}
TransformComponent::TransformComponent(const TransformComponentDescriptor &desc)
    : transform(desc.transform) {}
TransformComponent::~TransformComponent() {}
