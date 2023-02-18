#include "arenaalloc.hpp"

ArenaAllocator::ArenaAllocator(size_t size) {
  memory = std::vector<uint8_t>(size);
};

ArenaAllocator::~ArenaAllocator() {}

void *ArenaAllocator::alloc(size_t size) {
  uint8_t *ptr = &memory[head];
  head += size;
  return static_cast<void *>(ptr);
}

void *ArenaAllocator::realloc(void *original, size_t size) {
  (void)original;
  uint8_t *ptr = &memory[head];
  head += size;
  return static_cast<void *>(ptr);
}

void ArenaAllocator::free(void *ptr) { (void)ptr; }

void ArenaAllocator::reset() { head = 0; }