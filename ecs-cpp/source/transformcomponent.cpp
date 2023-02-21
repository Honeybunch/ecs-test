#include "transformcomponent.hpp"

TransformComponent::TransformComponent(const TransformComponentDescriptor &desc)
    : transform(desc.transform) {}
TransformComponent::~TransformComponent() {}
