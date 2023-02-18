cmake_minimum_required(VERSION 3.20)

project(ecs-c LANGUAGES C)

find_package(Threads REQUIRED)
find_package(Tracy CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)

file(GLOB_RECURSE source "source/*.c")
add_executable(ecs-c ${source})
target_link_libraries(ecs-c PRIVATE
  Tracy::TracyClient
  Threads::Threads
  $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
  $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>)
if(NOT MSVC)
  target_link_options(ecs-c PRIVATE -Wl,/subsystem:windows)
endif()
