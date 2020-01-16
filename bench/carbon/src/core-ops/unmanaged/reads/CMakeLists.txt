cmake_minimum_required(VERSION 3.9.6)

project(bench)

ADD_CUSTOM_TARGET(bench-reads)
add_dependencies(bench-reads karbonit-sources)
set_target_properties(bench-reads PROPERTIES EXCLUDE_FROM_ALL TRUE)

add_subdirectory(carbon/libcarbon)
add_subdirectory(bson/libbson)
