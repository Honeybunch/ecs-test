#pragma once

#include <cstdint>
#include <type_traits>

class AllocatorImpl {
public:
  AllocatorImpl() = default;
  AllocatorImpl(size_t size);
  virtual void *alloc(size_t size) = 0;
  virtual void *realloc(void *original, size_t size) = 0;
  virtual void free(void *ptr) = 0;
};

template <typename Impl> class Allocator {
  static_assert(std::is_base_of<AllocatorImpl, Impl>::value,
                "Impl must derive from AllocatorImpl");

public:
  Allocator() : impl(0) {}
  Allocator(size_t size) : impl(size) {}
  ~Allocator() {}

  template <typename T> T *alloc() {
    return static_cast<T *>(impl.alloc(sizeof(T)));
  }

  template <typename T> T *realloc(T *original) {
    return static_cast<T *>(impl.realloc(original, sizeof(T)));
  }

  template <typename T> T *alloc_num(uint32_t num) {
    return static_cast<T *>(impl.alloc(sizeof(T) * num));
  };

  template <typename T> T *realloc_num(T *original, uint32_t num) {
    return static_cast<T *>(impl.realloc(original, sizeof(T) * num));
  };

  template <typename T> void free(T *ptr) {
    impl.free(static_cast<void *>(ptr));
  }

  Impl &get_impl() { return impl; }

private:
  Impl impl;
};
