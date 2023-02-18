#pragma once

#include "allocator.hpp"

#include <vector>

class ArenaAllocator : public AllocatorImpl {
public:
  ArenaAllocator() = delete;
  ArenaAllocator(size_t size);
  ~ArenaAllocator();

  void *alloc(size_t size) override;
  void *realloc(void *original, size_t size) override;
  void free(void *ptr) override;

  void reset();

private:
  size_t head = 0;
  std::vector<uint8_t> memory;
};