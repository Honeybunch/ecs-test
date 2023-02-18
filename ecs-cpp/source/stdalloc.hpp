#pragma once

#include "allocator.hpp"

class StdAllocatorImpl : AllocatorImpl {
public:
  StdAllocatorImpl() = delete;
  StdAllocatorImpl(size_t size);
  ~StdAllocatorImpl();

  void *alloc(size_t size) override;
  void *realloc(void *original, size_t size) override;
  void free(void *ptr) override;

  void reset();
};

using StdAllocator = Allocator<StdAllocatorImpl>;
