cmake_minimum_required(VERSION 3.20)

project(ecs-cpp LANGUAGES CXX)

find_package(Threads REQUIRED)
find_package(Tracy CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)

file(GLOB_RECURSE source "source/*.cpp")
add_executable(ecs-cpp ${source})
target_link_libraries(ecs-cpp PRIVATE
  Tracy::TracyClient
  Threads::Threads
  $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
  $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>)
