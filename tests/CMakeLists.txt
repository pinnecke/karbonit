cmake_minimum_required(VERSION 3.9.6)

project(tests)
set (CMAKE_C_STANDARD 11)

ADD_CUSTOM_TARGET(tests)

macro(CreateTest arg)
    add_executable(${arg} ${arg}.cpp)
    target_link_libraries(${arg} PUBLIC gtest ${TEST_LIBS} PRIVATE jakson-sources)
    ADD_DEPENDENCIES(tests ${arg})
    add_test(${arg}  ${MAKE_DIRECTORY}/${arg})
endmacro()


set(
        TEST_LIBS
        gtest
        ${LIBS}
)

add_subdirectory("${CMAKE_SOURCE_DIR}/tests/carbon")
add_subdirectory("${CMAKE_SOURCE_DIR}/tests/misc")
