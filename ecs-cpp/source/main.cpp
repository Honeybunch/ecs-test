#include "arenaalloc.hpp"
#include "orbitcomponent.hpp"
#include "orbitsystem.hpp"
#include "stdalloc.hpp"
#include "transformcomponent.hpp"
#include "world.hpp"

#include <SDL2/SDL_main.h>
#include <SDL2/SDL_timer.h>
#include <tracy/Tracy.hpp>

struct AppSystems : public SystemStorage<OrbitSystem> {};
struct AppComponents
    : public ComponentStorage<TransformComponent, OrbitComponent> {};

int32_t SDL_main(int32_t argc, char *argv[]) {
  (void)argc;
  (void)argv;
  // Create allocators
  auto tmp_alloc = ArenaAllocator(512 * 1000 * 1000);
  ArenaAllocatorImpl &arena_alloc = tmp_alloc.get_impl();
  auto std_alloc = StdAllocator(0);

  auto world = World<AppSystems, AppComponents>(&std_alloc, &tmp_alloc);

  // Add a simple entity
  TransformComponentDescriptor trans_desc = {};
  OrbitComponentDescriptor orbit_desc = {};
  orbit_desc.center = {0, 0, 0};
  orbit_desc.distance = 100.0f;
  orbit_desc.axis = {0, 1, 0};
  orbit_desc.speed = 10.0f;
  const std::vector<const ComponentBase *> comp_descs = {
      &trans_desc,
      &orbit_desc,
  };
  world.add_entity(comp_descs);

  // Main loop
  bool running = true;

  uint64_t time = 0;
  uint64_t start_time = SDL_GetPerformanceCounter();
  uint64_t last_time = 0;
  uint64_t delta_time = 0;
  float delta_time_seconds = 0.0f;

  while (running) {
    ZoneScopedN("Main Loop");
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

    arena_alloc.reset();
    FrameMark;
  }

  return 0;
}