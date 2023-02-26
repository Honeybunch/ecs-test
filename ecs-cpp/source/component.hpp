#pragma once

#include "arenaalloc.hpp"
#include "stdalloc.hpp"
#include "system.hpp"

#include <stdint.h>

#include <span>

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

struct PackedComponentStoreBase {};

template <typename Comp>
struct PackedComponentStore : public PackedComponentStoreBase {
  std::span<Comp> components;
};

class ComponentStoreBase {
public:
  virtual void alloc_component() = 0;
  virtual void init_component(EntityId id,
                              const ComponentDescriptorBase *desc) = 0;

  virtual size_t get_store_idx() const = 0;

  virtual const PackedComponentStoreBase *
  pack_components(ArenaAllocator &tmp_alloc,
                  std::span<EntityId> entities) const = 0;

  virtual const Component *read_component(size_t idx) const = 0;
  virtual void write_components(const SystemWriteSetBase *set) = 0;
};

template <typename Comp, typename Desc, ComponentId cid>
class ComponentStore : public ComponentStoreBase {
  static_assert(std::is_base_of<Component, Comp>::value,
                "Comp must derive from Component");
  static_assert(std::is_base_of<ComponentDescriptorBase, Desc>::value,
                "Desc must derive from ComponentDescriptorBase");

public:
  ComponentStore(StdAllocator &alloc, size_t store_idx)
      : alloc(&alloc), store_idx(store_idx) {}

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

  size_t get_store_idx() const override { return store_idx; }

  const PackedComponentStoreBase *
  pack_components(ArenaAllocator &tmp_alloc,
                  std::span<EntityId> entities) const override {
    auto packed_store = tmp_alloc.alloc_new<PackedComponentStore<Comp>>();

    auto storage = tmp_alloc.alloc_span<Comp>(entities.size());
    size_t packed_idx = 0;
    for (EntityId i = 0; i < comp_count; ++i) {
      for (EntityId e : entities) {
        if (e == i) {
          storage[packed_idx++] = components[i];
          break;
        }
      }
    }
    packed_store->components = storage;

    return static_cast<const PackedComponentStoreBase *>(packed_store);
  };

  const Component *read_component(size_t idx) const override {
    return static_cast<const Component *>(&components[idx]);
  }
  void write_components(const SystemWriteSetBase *set) override {
    const auto *write_set =
        static_cast<const SystemWriteSet<Component, cid> *>(set);
    for (const auto &write : write_set->writes) {
      components[write.entity] = *static_cast<const Comp *>(write.component);
    }
  }

private:
  static constexpr size_t page_size = 256;
  StdAllocator *alloc;
  size_t comp_count = 0;
  Comp *components = nullptr;
  size_t comp_max = 0;
  size_t store_idx;
};
