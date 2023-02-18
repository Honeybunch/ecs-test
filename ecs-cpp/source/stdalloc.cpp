#include "stdalloc.hpp"

#include <memory>

StdAllocatorImpl::StdAllocatorImpl(size_t size) { (void)size; };

StdAllocatorImpl::~StdAllocatorImpl() {}

void *StdAllocatorImpl::alloc(size_t size) { return std::malloc(size); }

void *StdAllocatorImpl::realloc(void *original, size_t size) {
  return std::realloc(original, size);
}

void StdAllocatorImpl::free(void *ptr) { std::free(ptr); }
