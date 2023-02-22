#pragma once

#include "arenaalloc.hpp"
#include "stdalloc.hpp"

#include <span>
#include <stdint.h>
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
  EntityId id;
  const Comp *component;
};

template <typename Comp, ComponentId cid>
struct SystemWriteSet : public SystemWriteSetBase {
  const ComponentId id = cid;
  std::span<SystemWrite<Comp>> writes;
};

using SystemOutput = std::span<SystemWriteSetBase>;

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

protected:
  StdAllocator *std_alloc;
  ArenaAllocator *tmp_alloc;
};
