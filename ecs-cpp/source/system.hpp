#pragma once

#include "arenaalloc.hpp"
#include "stdalloc.hpp"

#include <stdint.h>

#include <span>
#include <vector>

using ComponentId = uint32_t;
using SystemId = uint32_t;
using EntityId = size_t;

struct PackedComponentStoreBase;
template <typename T> struct PackedComponentStore;

struct SystemInputSet {
  std::span<const PackedComponentStoreBase *> stores;
  std::span<EntityId> entities;
};

using SystemInput = std::span<SystemInputSet>;

struct SystemWriteSetBase {
  virtual ComponentId get_id() const = 0;
};

template <typename Comp> struct SystemWrite {
  EntityId entity;
  const Comp *component;
};

template <typename Comp, ComponentId cid>
struct SystemWriteSet : public SystemWriteSetBase {
  ComponentId get_id() const { return cid; }
  std::span<SystemWrite<Comp>> writes;
};

using SystemOutput = std::span<const SystemWriteSetBase *>;

using SystemInputQuery = std::span<ComponentId>;

using SystemInputQuerySet = std::span<SystemInputQuery>;

class System {
public:
  System() = delete;
  System(StdAllocator &std_alloc, ArenaAllocator &tmp_alloc)
      : std_alloc(&std_alloc), tmp_alloc(&tmp_alloc) {}

  virtual SystemInputQuerySet get_input_queries() const = 0;
  virtual const SystemOutput &tick(const SystemInput &input,
                                   float delta_seconds) = 0;

  template <typename T>
  PackedComponentStore<T> &
  make_out_copy(const PackedComponentStoreBase *input) {
    const PackedComponentStore<T> *in =
        static_cast<const PackedComponentStore<T> *>(input);

    auto *out_copy = tmp_alloc->alloc<PackedComponentStore<T>>();
    const auto comp_count = in->components.size();
    auto out_comps = tmp_alloc->alloc_span<T>(comp_count);
    for (size_t i = 0; i < comp_count; ++i) {
      out_comps[i] = in->components[i];
    }

    out_copy->components = out_comps;
    return *out_copy;
  }

protected:
  StdAllocator *std_alloc;
  ArenaAllocator *tmp_alloc;
};
