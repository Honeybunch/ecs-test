#include "arenaalloc.hpp"

ArenaAllocatorImpl::ArenaAllocatorImpl(size_t size) {
  memory = std::vector<uint8_t>(size);
};

ArenaAllocatorImpl::~ArenaAllocatorImpl() {}

void *ArenaAllocatorImpl::alloc(size_t size) {
  uint8_t *ptr = &memory[head];
  head += size;
  return static_cast<void *>(ptr);
}

void *ArenaAllocatorImpl::realloc(void *original, size_t size) {
  (void)original;
  uint8_t *ptr = &memory[head];
  head += size;
  return static_cast<void *>(ptr);
}

void ArenaAllocatorImpl::free(void *ptr) { (void)ptr; }

void ArenaAllocatorImpl::reset() { head = 0; }