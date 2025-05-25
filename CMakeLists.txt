cmake_minimum_required(VERSION 3.10)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Debug)

project(ecs)
add_library(${PROJECT_NAME} SHARED)
file(GLOB SRC ./src/*)
target_sources(${PROJECT_NAME} PUBLIC ${SRC})

project(test)
file(GLOB SRC ./examples/test*)
add_executable(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME} ecs)
