#include "arenaalloc.hpp"

ArenaAllocatorImpl::ArenaAllocatorImpl(size_t size) {
  memory = new uint8_t[size];
};

ArenaAllocatorImpl::~ArenaAllocatorImpl() { delete[] memory; }

void *ArenaAllocatorImpl::alloc(size_t size) {
  uint8_t *ptr = &memory[head];

  // Always 16 byte aligned
  intptr_t padding = 0;
  if ((intptr_t)ptr % 16 != 0) {
    padding = (16 - (intptr_t)ptr % 16);
  }
  ptr = (uint8_t *)((intptr_t)ptr + padding);

  head += (size + padding);
  return static_cast<void *>(ptr);
}

void *ArenaAllocatorImpl::realloc(void *original, size_t size) {
  (void)original;
  return alloc(size);
}

void ArenaAllocatorImpl::free(void *ptr) { (void)ptr; }

void ArenaAllocatorImpl::reset() { head = 0; }