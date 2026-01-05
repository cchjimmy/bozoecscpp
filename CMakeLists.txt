cmake_minimum_required(VERSION 3.10)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Debug)
set(BUILD_SHARED_LIBS ON)

include(FetchContent)

FetchContent_Declare(
	raylib
	GIT_REPOSITORY https://github.com/raysan5/raylib.git
)

FetchContent_MakeAvailable(
	raylib
)

file(GLOB EXAMPLES ./examples/*)
foreach(EXAMPLE ${EXAMPLES})
	if(IS_DIRECTORY ${EXAMPLE})
		add_subdirectory(${EXAMPLE})
	endif()
endforeach()
