#pragma once

#include "allocator.h"
#include "simd.h"

#define OrbitSystemId 0xDEADFF00

typedef struct SystemDescriptor SystemDescriptor;

typedef struct OrbitSystemDescriptor {
  Allocator tmp_alloc;
} OrbitSystemDescriptor;

typedef struct OrbitSystem {
  Allocator tmp_alloc;
} OrbitSystem;

void tb_orbit_system_descriptor(SystemDescriptor *desc,
                                const OrbitSystemDescriptor *sys_desc);
