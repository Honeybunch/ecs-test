#include "application.hpp"

#include "allocator.hpp"
#include "world.hpp"

#include <SDL2/SDL_timer.h>
#include <tracy/Tracy.hpp>

Application::Application(StdAllocator &std_alloc, ArenaAllocator &tmp_alloc,
                         std::span<World *> worlds) {
  // Main loop
  bool running = true;
  ArenaAllocatorImpl &arena_alloc = tmp_alloc.get_impl();

  uint64_t time = 0;
  uint64_t start_time = SDL_GetPerformanceCounter();
  uint64_t last_time = 0;
  uint64_t delta_time = 0;
  float delta_time_seconds = 0.0f;

  while (running) {
    ZoneScopedN("Application::Application");
    // Use SDL High Performance Counter to get timing info
    time = SDL_GetPerformanceCounter() - start_time;
    delta_time = time - last_time;
    delta_time_seconds =
        (float)((double)delta_time / (double)(SDL_GetPerformanceFrequency()));
    last_time = time;

    // Tick the worlds
    for (World *world : worlds) {
      if (!world->tick(delta_time_seconds)) {
        running = false;
        break;
      }
    }

    arena_alloc.reset();
    FrameMark;
  }
}