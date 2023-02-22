#pragma once

#include "arenaalloc.hpp"
#include "stdalloc.hpp"

#include <span>
#include <stdint.h>
#include <vector>

using ComponentId = uint32_t;
using SystemId = uint32_t;
using EntityId = size_t;

struct InputComponentStore {
  ComponentId id;

  size_t count;
  size_t comp_size;
  void *components;
};

struct SystemInputSet {
  std::span<InputComponentStore> stores;
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

struct SystemInputQuery : public std::span<ComponentId> {};

struct SystemInputQuerySet : public std::span<SystemInputQuery> {};

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
