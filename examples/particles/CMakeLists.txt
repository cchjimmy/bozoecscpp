cmake_minimum_required(VERSION 3.10)

add_executable(particles)
file(GLOB SRCS ./src/*)

target_sources(particles PUBLIC ${SRCS})
target_link_libraries(particles ecs raylib)
target_include_directories(particles PUBLIC ../../src)
