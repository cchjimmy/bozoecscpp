cmake_minimum_required(VERSION 3.10)

project(particles)
add_executable(${PROJECT_NAME})
file(GLOB SRCS ./src/*)
target_sources(${PROJECT_NAME} PUBLIC ${SRCS})
target_link_libraries(${PROJECT_NAME} raylib)
target_include_directories(${PROJECT_NAME} PUBLIC ../../src)
