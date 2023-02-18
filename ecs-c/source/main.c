#include "allocator.h"
#include "orbitcomponent.h"
#include "orbitsystem.h"
#include "profiling.h"
#include "transformcomponent.h"
#include "world.h"

#include <SDL2/SDL_main.h>
#include <SDL2/SDL_timer.h>
#include <string.h>

int32_t SDL_main(int32_t argc, char *argv[]) {
  (void)argc;
  (void)argv;

  {
    const char *app_info = "Debug";
    size_t app_info_len = strlen(app_info);
    TracyCAppInfo(app_info, app_info_len)(void) app_info_len;

    TracyCSetThreadName("Main Thread");
  }

  // Create Temporary Arena Allocator
  ArenaAllocator arena = {0};
  {
    const size_t arena_alloc_size = 1024 * 1024 * 512; // 512 MB
    create_arena_allocator("Main Arena", &arena, arena_alloc_size);
  }

  StandardAllocator std_alloc = {0};
  create_standard_allocator(&std_alloc, "std_alloc");

// Order does not matter
#define COMP_COUNT 2
  ComponentDescriptor component_descs[COMP_COUNT] = {0};
  {
    int32_t i = 0;
    tb_transform_component_descriptor(&component_descs[i++]);
    tb_orbit_component_descriptor(&component_descs[i++]);
  }

// Order doesn't matter here
#define SYSTEM_COUNT 1
  SystemDescriptor system_descs[SYSTEM_COUNT] = {0};
  {
    uint32_t i = 0;
    OrbitSystemDescriptor orbit_system_desc = {
        .tmp_alloc = arena.alloc,
    };
    tb_orbit_system_descriptor(&system_descs[i++], &orbit_system_desc);
  }

  // But it does matter here
  SystemId init_order[SYSTEM_COUNT];
  {
    uint32_t i = 0;
    init_order[i++] = OrbitSystemId;
  }
  SystemId tick_order[SYSTEM_COUNT];
  {
    uint32_t i = 0;
    tick_order[i++] = OrbitSystemId;
  }

  WorldDescriptor world_desc = {
      .std_alloc = std_alloc.alloc,
      .tmp_alloc = arena.alloc,
      .component_count = COMP_COUNT,
      .component_descs = component_descs,
      .system_count = SYSTEM_COUNT,
      .system_descs = system_descs,
      .init_order = init_order,
      .tick_order = tick_order,
  };
#undef COMP_COUNT
#undef SYSTEM_COUNT

  World world = {0};
  bool success = tb_create_world(&world_desc, &world);

  TransformComponentDescriptor trans_comp_desc = {
      .world = &world,
      .transform = {
          .position = (float3){0},
          .rotation = (Quaternion){0, 0, 0, 1},
          .scale = (float3){1, 1, 1},
      }};
  OrbitComponentDescriptor orbit_comp_desc = {
      .center = (float3){0},
      .distance = 100.0f,
      .axis = (float3){0.0f, 1.0f, 0.0f},
      .speed = 10.0f,
  };
  ComponentId core_comp_ids[2] = {TransformComponentId, OrbitComponentId};
  InternalDescriptor core_comp_descs[2] = {
      &trans_comp_desc,
      &orbit_comp_desc,
  };
  EntityDescriptor entity_desc = {
      .name = "Core",
      .component_count = 2,
      .component_ids = core_comp_ids,
      .component_descriptors = core_comp_descs,
  };
  tb_world_add_entity(&world, &entity_desc);

  // Main loop
  bool running = true;

  uint64_t time = 0;
  uint64_t start_time = SDL_GetPerformanceCounter();
  uint64_t last_time = 0;
  uint64_t delta_time = 0;
  float delta_time_seconds = 0.0f;

  while (running) {
    TracyCFrameMarkStart("Simulation Frame");
    TracyCZoneN(trcy_ctx, "Simulation Frame", true);
    TracyCZoneColor(trcy_ctx, TracyCategoryColorCore);

    // Use SDL High Performance Counter to get timing info
    time = SDL_GetPerformanceCounter() - start_time;
    delta_time = time - last_time;
    delta_time_seconds =
        (float)((double)delta_time / (double)(SDL_GetPerformanceFrequency()));
    last_time = time;

    // Tick the world
    if (!tb_tick_world(&world, delta_time_seconds)) {
      running = false;
      TracyCZoneEnd(trcy_ctx);
      TracyCFrameMarkEnd("Simulation Frame");
      break;
    }

    // Reset the arena allocator
    arena = reset_arena(arena);

    TracyCZoneEnd(trcy_ctx);
    TracyCFrameMarkEnd("Simulation Frame");
  }

  tb_destroy_world(&world);

  destroy_arena_allocator(arena);
  destroy_standard_allocator(std_alloc);

  return 0;
}