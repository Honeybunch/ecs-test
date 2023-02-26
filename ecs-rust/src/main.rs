mod world;

use bumpalo::Bump;
use sdl2::sys::*;
use tracy_client::*;

use world::World;

// For Tracy memory tracking
#[global_allocator]
static GLOBAL: ProfiledAllocator<std::alloc::System> =
    ProfiledAllocator::new(std::alloc::System, 100);

fn main() {
    // Start Tracy
    let tracy = Client::start();

    // Create Arena Allocator
    let mut arena = Bump::with_capacity(512 * 1000 * 1000);

    // Register component types
    // Register system types

    // Create world
    let mut world = World::new();

    // Create test entity

    // Main loop
    let start_time = unsafe { SDL_GetPerformanceCounter() };
    let mut time: u64;
    let mut last_time: u64 = 0;
    let mut delta_time: u64;
    let mut delta_seconds: f64;

    let mut running = true;
    while running {
        span!("Main Loop", 100);

        time = unsafe { SDL_GetPerformanceCounter() } - start_time;
        delta_time = time - last_time;
        delta_seconds = delta_time as f64 / unsafe { SDL_GetPerformanceFrequency() } as f64;
        last_time = time;

        // Tick world
        running = world.tick(delta_seconds as f32);

        arena.reset();

        tracy.frame_mark();
    }

    // Cleanup

    println!("Hello, world!");
}
