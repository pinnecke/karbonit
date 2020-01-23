cmake_minimum_required(VERSION 3.5)
project(bench-reads-libbson C)

link_directories("/usr/local/lib/")

find_package (libbson-1.0 1.7 REQUIRED)

message ("--   libbson found version \"${BSON_VERSION}\"")
message ("--   libbson include path \"${BSON_INCLUDE_DIRS}\"")
message ("--   libbson libraries \"${BSON_LIBRARIES}\"")

set(BENCH_SOURCES
        ../../read_bench.h
        ../../read_bench.c)

message("Bench sources: ${BENCH_SOURCES}")

add_executable(bench-reads-libbson main.c ${BENCH_SOURCES} ${TOOL_SOURCES})
target_link_libraries(bench-reads-libbson PUBLIC ${LIBS} PRIVATE karbonit-sources)

target_include_directories (bench-reads-libbson PRIVATE ${BSON_INCLUDE_DIRS})
target_link_libraries (bench-reads-libbson PRIVATE ${BSON_LIBRARIES})
target_compile_definitions (bench-reads-libbson PRIVATE ${BSON_DEFINITIONS})
