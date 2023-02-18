#include "world.hpp"

#include <SDL2/SDL_main.h>
#include <SDL2/SDL_timer.h>
#include <tracy/Tracy.hpp>

int32_t SDL_main(int32_t argc, char *argv[]) {
  (void)argc;
  (void)argv;

  World world();
  // Main loop
  bool running = true;

  uint64_t time = 0;
  uint64_t start_time = SDL_GetPerformanceCounter();
  uint64_t last_time = 0;
  uint64_t delta_time = 0;
  float delta_time_seconds = 0.0f;

  while (running) {
    // Use SDL High Performance Counter to get timing info
    time = SDL_GetPerformanceCounter() - start_time;
    delta_time = time - last_time;
    delta_time_seconds =
        (float)((double)delta_time / (double)(SDL_GetPerformanceFrequency()));
    last_time = time;

    // Tick the world
    if (!world.tick(delta_time_seconds)) {
      running = false;
      break;
    }

    FrameMark;
  }

  return 0;
}