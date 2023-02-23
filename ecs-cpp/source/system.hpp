#pragma once

#include "arenaalloc.hpp"
#include "component.hpp"
#include "stdalloc.hpp"

#include <stdint.h>

#include <span>
#include <vector>

using ComponentId = uint32_t;
using SystemId = uint32_t;
using EntityId = size_t;

struct PackedComponentStoreBase;

struct SystemInputSet {
  std::span<const PackedComponentStoreBase *> stores;
  std::span<EntityId> entities;
};

using SystemInput = std::span<SystemInputSet>;

struct SystemWriteSetBase {};

template <typename Comp> struct SystemWrite {
  EntityId entity;
  const Comp *component;
};

template <typename Comp, ComponentId cid>
struct SystemWriteSet : public SystemWriteSetBase {
  const ComponentId id = cid;
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
    T *out_comps = tmp_alloc->alloc_num<T>(comp_count);
    SDL_memcpy(out_comps, in->components.data(), comp_count * sizeof(T));

    out_copy->components = std::span<T>(out_comps, comp_count);
    return *out_copy;
  }

protected:
  StdAllocator *std_alloc;
  ArenaAllocator *tmp_alloc;
};
