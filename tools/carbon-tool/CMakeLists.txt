cmake_minimum_required(VERSION 3.5)
project(karbonit-tool C)

set(TOOL_SOURCES
        modules.h
        modules.c)

add_executable(carbon-tool main.c ${TOOL_SOURCES})
target_link_libraries(carbon-tool PUBLIC ${LIBS} PRIVATE karbonit-sources)

install(TARGETS carbon-tool
        RUNTIME DESTINATION bin)