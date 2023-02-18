#include "allocator.h"

#include <assert.h>
#include <malloc.h>
#include <string.h>

#include "profiling.h"

static void *arena_alloc(void *user_data, size_t size) {
  TracyCZone(ctx, true);
  TracyCZoneColor(ctx, TracyCategoryColorMemory);
  ArenaAllocator *arena = (ArenaAllocator *)user_data;
  size_t cur_size = arena->size;
  void *ptr = &arena->data[cur_size];

  // Always 16 byte aligned
  intptr_t padding = 0;
  if ((intptr_t)ptr % 16 != 0) {
    padding = (16 - (intptr_t)ptr % 16);
  }
  ptr = (void *)((intptr_t)ptr + padding);

  arena->size += (size + padding);
  TracyCZoneEnd(ctx);
  return ptr;
}

static void *arena_realloc(void *user_data, void *original, size_t size) {
  // In the arena allocator we're not going to bother to really implement
  // realloc for now...
  (void)original;
  return arena_alloc(user_data, size);
}

static void *arena_realloc_aligned(void *user_data, void *original, size_t size,
                                   size_t alignment) {
  // In the arena allocator we're not going to bother to really implement
  // realloc for now...
  (void)original;
  (void)alignment;
  return arena_alloc(user_data, size);
}

static void arena_free(void *user_data, void *ptr) {
  // Do nothing, the arena will reset
  (void)user_data;
  (void)ptr;
}

void create_arena_allocator(const char *name, ArenaAllocator *a,
                            size_t max_size) {

  void *data = realloc(NULL, max_size);
  TracyCAllocN(data, max_size, name);
  assert(data);

  (*a) = (ArenaAllocator){
      .name = name,
      .max_size = max_size,
      .data = data,
      .alloc =
          (Allocator){
              .alloc = arena_alloc,
              .realloc = arena_realloc,
              .realloc_aligned = arena_realloc_aligned,
              .free = arena_free,
              .user_data = a,
          },
  };
}

ArenaAllocator reset_arena(ArenaAllocator a) {
  TracyCZone(ctx, true);
  TracyCZoneColor(ctx, TracyCategoryColorMemory);
  a.size = 0;

  assert(a.data);
  TracyCZoneEnd(ctx);
  return a;
}

void destroy_arena_allocator(ArenaAllocator a) {
  TracyCFreeN(a.data, a.name);
  free(a.data);
}

static void *standard_alloc(void *user_data, size_t size) {
  TracyCZone(ctx, true);
  TracyCZoneColor(ctx, TracyCategoryColorMemory);
  StandardAllocator *alloc = (StandardAllocator *)user_data;
  void *ptr = realloc(NULL, size);
  TracyCAllocN(ptr, size, alloc->name);
  TracyCZoneEnd(ctx);
  return ptr;
}

static void *standard_realloc(void *user_data, void *original, size_t size) {
  TracyCZone(ctx, true);
  TracyCZoneColor(ctx, TracyCategoryColorMemory);
  StandardAllocator *alloc = (StandardAllocator *)user_data;
  TracyCFreeN(original, alloc->name);
  void *ptr = realloc(original, size);
  TracyCAllocN(ptr, size, alloc->name);
  TracyCZoneEnd(ctx);
  return ptr;
}

static void *standard_realloc_aligned(void *user_data, void *original,
                                      size_t size, size_t alignment) {
  TracyCZone(ctx, true);
  TracyCZoneColor(ctx, TracyCategoryColorMemory);
  StandardAllocator *alloc = (StandardAllocator *)user_data;
  TracyCFreeN(original, alloc->name);
  void *ptr = realloc(original, size);
  TracyCAllocN(ptr, size, alloc->name);
  TracyCZoneEnd(ctx);
  return ptr;
}

static void standard_free(void *user_data, void *ptr) {
  TracyCZone(ctx, true);
  TracyCZoneColor(ctx, TracyCategoryColorMemory);
  StandardAllocator *alloc = (StandardAllocator *)user_data;
  (void)alloc;
  TracyCFreeN(ptr, alloc->name);
  free(ptr);
  TracyCZoneEnd(ctx);
}

void create_standard_allocator(StandardAllocator *a, const char *name) {
  (*a) = (StandardAllocator){
      .alloc =
          {
              .alloc = standard_alloc,
              .realloc = standard_realloc,
              .realloc_aligned = standard_realloc_aligned,
              .free = standard_free,
              .user_data = a,
          },
      .name = name,
  };
}

void destroy_standard_allocator(StandardAllocator a) { (void)a; }
