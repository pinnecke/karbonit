cmake_minimum_required(VERSION 3.5)
project(bench-reads-libcarbon C)

link_directories("/usr/local/lib/")

set(BENCH_SOURCES
        ../../read_bench.h
        ../../read_bench.c)

message("Bench sources: ${BENCH_SOURCES}")

add_executable(bench-reads-libcarbon main.c ${BENCH_SOURCES} ${TOOL_SOURCES})
target_link_libraries(bench-reads-libcarbon PUBLIC ${LIBS} PRIVATE karbonit-sources)
