#pragma once

#include <stdint.h>

using ComponentId = uint32_t;

struct ComponentBase {
  virtual constexpr ComponentId get_id() const = 0;
};

template <ComponentId cid> struct ComponentDescriptor : public ComponentBase {
  constexpr ComponentId get_id() const override { return cid; }
};

template <ComponentId cid> class Component : public ComponentBase {
public:
  constexpr ComponentId get_id() const override { return cid; }
};
