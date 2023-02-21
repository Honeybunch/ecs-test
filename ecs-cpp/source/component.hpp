#pragma once

#include "stdalloc.hpp"

#include <stdint.h>

using EntityId = size_t;
using ComponentId = uint32_t;

class Component {};

struct ComponentDescriptor {
  ComponentId id;
};

class ComponentStoreBase {
public:
  virtual void alloc_component() = 0;
  virtual void init_component(EntityId id, const ComponentDescriptor *desc) = 0;
};

template <typename Comp, typename Desc>
class ComponentStore : public ComponentStoreBase {
public:
  ComponentStore(StdAllocator &alloc) : alloc(&alloc) {}

  void alloc_component() override {
    const size_t next_count = comp_count + 1;
    if (next_count > comp_max) {
      comp_max = next_count * 2;
      components = alloc->realloc_num<Comp>(components, comp_max);
    }
    comp_count = next_count;
  }

  void init_component(EntityId id, const ComponentDescriptor *desc) override {
    Comp &comp = components[id];
    new (&comp) Comp(*static_cast<const Desc *>(desc));
  }

private:
  StdAllocator *alloc;
  size_t comp_count = 0;
  Comp *components = nullptr;
  size_t comp_max = 0;
};
