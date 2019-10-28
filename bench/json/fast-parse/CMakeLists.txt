cmake_minimum_required(VERSION 3.5)
project(bench-json-fast-parse CXX)

set (CMAKE_CXX_STANDARD 11)

add_executable(bench-json-fast-parse main.cpp)
target_link_libraries(bench-json-fast-parse PRIVATE jakson-sources)

ADD_DEPENDENCIES(bench-json bench-json-fast-parse)
