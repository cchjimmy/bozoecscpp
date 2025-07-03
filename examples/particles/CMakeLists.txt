cmake_minimum_required(VERSION 3.10)

add_executable(particles)
file(GLOB SRCS ./src/*)

include(FetchContent)

FetchContent_Declare(
	raylib
	GIT_REPOSITORY https://github.com/raysan5/raylib.git
)

FetchContent_MakeAvailable(
	raylib
)

target_sources(particles PUBLIC ${SRCS})
target_link_libraries(particles ecs raylib)
target_include_directories(particles PUBLIC ../../src)
