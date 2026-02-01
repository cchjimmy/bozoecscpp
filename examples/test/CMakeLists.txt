cmake_minimum_required(VERSION 3.10)

project(test)
add_executable(${PROJECT_NAME})
file(GLOB SRCS ./src/*)
target_sources(${PROJECT_NAME} PRIVATE ${SRCS})
