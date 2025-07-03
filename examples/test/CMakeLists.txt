cmake_minimum_required(VERSION 3.10)

add_executable(test)
file(GLOB SRCS ./src/* )
target_sources(test PUBLIC ${SRCS})
target_link_libraries(test ecs)
target_include_directories(test PUBLIC ../../src)
