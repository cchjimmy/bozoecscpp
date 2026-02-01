cmake_minimum_required(VERSION 3.10)

project(withSdl3)
add_executable(${PROJECT_NAME})
file(GLOB SRCS ./src/*)
target_sources(${PROJECT_NAME} PRIVATE ${SRCS})
find_package(SDL3 REQUIRED CONFIG COMPONENTS SDL3-shared)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3)
