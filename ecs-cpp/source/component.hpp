#pragma once

#include "stdalloc.hpp"

#include <stdint.h>

using EntityId = size_t;
using ComponentId = uint32_t;

class Component {};

struct ComponentDescriptorBase {
  virtual ComponentId get_id() const = 0;
};

template <ComponentId cid>
struct ComponentDescriptor : public ComponentDescriptorBase {
  ComponentId get_id() const override { return cid; }
};

class ComponentStoreBase {
public:
  virtual void alloc_component() = 0;
  virtual void init_component(EntityId id,
                              const ComponentDescriptorBase *desc) = 0;
};

template <typename Comp, typename Desc>
class ComponentStore : public ComponentStoreBase {
  static_assert(std::is_base_of<Component, Comp>::value,
                "Comp must derive from Component");
  static_assert(std::is_base_of<ComponentDescriptorBase, Desc>::value,
                "Desc must derive from ComponentDescriptorBase");

public:
  ComponentStore(StdAllocator &alloc) : alloc(&alloc) {}

  void alloc_component() override {
    const size_t next_count = comp_count + 1;
    if (next_count > comp_max) {
      comp_max += page_size;
      components = alloc->realloc_num<Comp>(components, comp_max);
    }
    comp_count = next_count;
  }

  void init_component(EntityId id,
                      const ComponentDescriptorBase *desc) override {
    Comp &comp = components[id];
    new (&comp) Comp(*static_cast<const Desc *>(desc));
  }

private:
  static constexpr size_t page_size = 256;
  StdAllocator *alloc;
  size_t comp_count = 0;
  Comp *components = nullptr;
  size_t comp_max = 0;
};
