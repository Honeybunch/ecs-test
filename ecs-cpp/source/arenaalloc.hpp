#pragma once

#include "allocator.hpp"

#include <vector>

class ArenaAllocatorImpl : public AllocatorImpl {
public:
  ArenaAllocatorImpl() = delete;
  ArenaAllocatorImpl(size_t size);
  ~ArenaAllocatorImpl();

  void *alloc(size_t size) override;
  void *realloc(void *original, size_t size) override;
  void free(void *ptr) override;

  void reset();

private:
  size_t head = 0;
  uint8_t *memory;
};

using ArenaAllocator = Allocator<ArenaAllocatorImpl>;