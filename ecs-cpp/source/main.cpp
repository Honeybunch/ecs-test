#include "application.hpp"
#include "arenaalloc.hpp"
#include "orbitcomponent.hpp"
#include "orbitsystem.hpp"
#include "stdalloc.hpp"
#include "transformcomponent.hpp"
#include "world.hpp"

#include <SDL2/SDL_main.h>

int32_t SDL_main(int32_t argc, char *argv[]) {
  (void)argc;
  (void)argv;

  auto tmp_alloc = ArenaAllocator(512 * 1000 * 1000);
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

  std::array<const ComponentDescriptorBase *, 2> comp_descs = {
      &transform_comp_desc,
      &orbit_comp_desc,
  };
  world.add_entity(comp_descs);

  // Construct application that will tick this world
  std::array<World *, 1> worlds = {&world};
  Application app(std_alloc, tmp_alloc, worlds);

  return 0;
}