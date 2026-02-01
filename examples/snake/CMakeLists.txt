cmake_minimum_required(VERSION 3.10)

project(snake)
add_executable(${PROJECT_NAME})
file(GLOB SRCS ./src/*)
find_package(raylib)
target_sources(${PROJECT_NAME} PRIVATE ${SRCS})
target_link_libraries(${PROJECT_NAME} raylib)
