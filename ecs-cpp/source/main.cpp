#include "arenaalloc.hpp"
#include "orbitcomponent.hpp"
#include "orbitsystem.hpp"
#include "stdalloc.hpp"
#include "transformcomponent.hpp"
#include "world.hpp"

#include <SDL2/SDL_main.h>
#include <SDL2/SDL_timer.h>
#include <tracy/Tracy.hpp>

int32_t SDL_main(int32_t argc, char *argv[]) {
  (void)argc;
  (void)argv;
  // Create allocators
  auto tmp_alloc = ArenaAllocator(512 * 1000 * 1000);
  ArenaAllocatorImpl &arena_alloc = tmp_alloc.get_impl();
  auto std_alloc = StdAllocator(0);

  WorldDescriptor world_desc = {
      .std_alloc = &std_alloc,
      .tmp_alloc = &tmp_alloc,
  };
  auto world = World(world_desc);
  world.register_system<OrbitSystemId, OrbitSystem>();
  world.register_component<TransformComponentId, TransformComponentStore>();
  world.register_component<OrbitComponentId, OrbitComponentStore>();

  // Add a simple entity
  auto transform_comp_desc = TransformComponentDescriptor();
  auto orbit_comp_desc = OrbitComponentDescriptor();
  orbit_comp_desc.distance = 100.0f;
  orbit_comp_desc.axis = float3{0.0f, 1.0f, 0.0f};
  orbit_comp_desc.speed = 10.0f;

  std::array<const ComponentDescriptor *, 2> comp_descs = {
      &transform_comp_desc,
      &orbit_comp_desc,
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