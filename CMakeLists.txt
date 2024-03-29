cmake_minimum_required(VERSION 3.9.6)

project(karbonit VERSION 1.0.0.0 DESCRIPTION "A library to operate with CARBON files"
        LANGUAGES C) # VERSION major.minor.patch.tweak

set (CMAKE_C_STANDARD 11)

set(MAKE_DIRECTORY "${CMAKE_HOME_DIRECTORY}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${MAKE_DIRECTORY})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${MAKE_DIRECTORY})

include_directories("${CMAKE_SOURCE_DIR}/src")
include_directories("${CMAKE_SOURCE_DIR}/include")
include_directories("${CMAKE_SOURCE_DIR}")

include_directories("/usr/include")
include_directories("/usr/local/include")

set(LIBS ${LIBS} pthread m)

file(GLOB_RECURSE LIB_LIBS_HEADER_FILES libs/*.h )
file(GLOB_RECURSE LIB_LIBS_SOURCE_FILES libs/*.c )

#
# Source Amalgamation
#
# In order to allow the C compiler to optimize across translation unit boundaries, the optimized release version of
# Karbonit combines all source files (in 'src/') by a pre-building process into one translation unit, `karbonit.h` and
# `karbonit.c` located in `include`. This technique, called amalgamation, is inspired by SQLite and others Duktape, which
# do similar. Amalgamation in its simplest form is realy just that: source concatenation. In order to let this
# concatenation runs smoothly, implemenations files must make use of static functions and must avoid naming conflicts
# (typically by the use of `__` prefix in implementation files).
#
# The implementation of amalgamation for Karbonit is done as follows. Independent of the build type, amalgamation can
# turned off or on. If it is turned off, a header file 'karbonit.h' is generated in `include/karbonit` that includes all
# header files in 'src/', and each source file in 'src/' is compiled as a single translation unit when compiled.
# If amalgamation is turned on, all header files in 'src/' are concatenated as a single file 'karbonit.h' in
# `include/karbonit`, and all implementation files in 'src/' are concatenated as a single file 'karbonit.c'. The original
# `src/` directory is ignored when compilation is done, leading to one large compilation unit stored in
# `include/karbonit`.
#
# By including `include/karbonit` instead of indivudal header files, the transition between builds with and without
# amalgamation is smoothly.
#

set(SRC_IN "${CMAKE_SOURCE_DIR}/src")
set(SRC_TEMP "${CMAKE_SOURCE_DIR}/src-tmp")
set(SRC_AMALGAMATION "${CMAKE_SOURCE_DIR}/include/karbonit")

file(GLOB_RECURSE LIB_HEADER_FILES ${SRC_IN}/*.h )
file(GLOB_RECURSE LIB_SOURCE_FILES ${SRC_IN}/*.c )

set(LIB_SOURCES ${LIB_LIBS_HEADER_FILES} ${LIB_LIBS_SOURCE_FILES})

function(cat IN_FILE OUT_FILE)
    file(READ ${IN_FILE} CONTENTS)
    file(APPEND ${OUT_FILE} "${CONTENTS}")
endfunction()

if (CMAKE_BUILD_TYPE MATCHES Debug OR BUILD_TYPE MATCHES Debug)
    message("Debug build is enabled")
    set(CMAKE_BUILD_TYPE Debug)
elseif(CMAKE_BUILD_TYPE MATCHES Release OR BUILD_TYPE MATCHES Release)
    message("Release build is enabled")
    set(CMAKE_BUILD_TYPE Release)
endif()

OPTION(USE_AMALGAMATION "Turns source amalgamation (source code concatenation) on or off" ON)

# cleanup old amalgamation files
file(GLOB_RECURSE SRC_AMALGAMATION_FILES ${SRC_AMALGAMATION}/*)
message("-- *** cleanup amalgamation output ***")
foreach(FILE ${SRC_AMALGAMATION_FILES})
    file(REMOVE FILE)
    message("     - removed ${FILE}")
endforeach()


if (USE_AMALGAMATION)
    message("-- *** source amalgamation is turned on ***")
    message("     - concat '${SRC_IN}/*.h' to '${SRC_AMALGAMATION}/karbonit.h'")
    message("     - concat '${SRC_IN}/*.c' to '${SRC_AMALGAMATION}/karbonit.c'")

    message("-- Building header file 'karbonit.h'")
    file(WRITE ${SRC_AMALGAMATION}/karbonit.h "")
    foreach(LIB_HEADER_FILE ${LIB_HEADER_FILES})
        cat(${LIB_HEADER_FILE} ${SRC_AMALGAMATION}/karbonit.h)
        message("     - append ${LIB_HEADER_FILE}")
    endforeach()

    message("-- Building implementation file 'karbonit.c'")
    file(WRITE ${SRC_AMALGAMATION}/karbonit.c "")
    foreach(LIB_SOURCE_FILE ${LIB_SOURCE_FILES})
        cat(${LIB_SOURCE_FILE} ${SRC_AMALGAMATION}/karbonit.c)
        message("     - append ${LIB_SOURCE_FILE}")
    endforeach()

    message("-- Inclusion header 'karbonit.h' has been successfully built")
    message("-- Implementation file 'karbonit.c' has been successfully built")

    set(LIB_SOURCES ${SRC_AMALGAMATION}/karbonit.h ${SRC_AMALGAMATION}/karbonit.c ${LIB_SOURCES})
    message("-- Source files are: ")
    foreach(LIB_SOURCE_FILE ${LIB_SOURCES})
        message("     - '${LIB_SOURCE_FILE}'")
    endforeach()
else()
    message("-- *** source amalgamation is turned off ***")

    message("-- Generating inclusion header 'karbonit.h' in ${SRC_AMALGAMATION}/")

    file(WRITE ${SRC_AMALGAMATION}/karbonit.h "/* This is the generated inclusion file of Karbonit. Do not modify the contents. */\n#ifndef JAKSON\n#define JAKSON\n")

    foreach(LIB_HEADER_FILE ${LIB_HEADER_FILES})
        file(APPEND ${SRC_AMALGAMATION}/karbonit.h "#include \"${LIB_HEADER_FILE}\"\n")
        message("     - include ${LIB_HEADER_FILE}")
    endforeach()

    file(APPEND ${SRC_AMALGAMATION}/karbonit.h "#endif")

    message("-- Inclusion header 'karbonit.h' has been successfully built")

    set(LIB_SOURCES ${SRC_AMALGAMATION}/karbonit.h ${LIB_HEADER_FILES} ${LIB_SOURCE_FILES} ${LIB_SOURCES})

    message("-- Source files are: ")
    foreach(LIB_SOURCE_FILE ${LIB_SOURCES})
        message("     - '${LIB_SOURCE_FILE}'")
    endforeach()

endif()

# End of Source Amalgamation

if (${LOG_INFO} MATCHES "on")
    if (${BUILD_TYPE} MATCHES Debug)
        message("-- Info log is enabled")
        add_definitions(-DLOG_INFO)
    else()
        message("-- ** WARNING ** Info log is enabled but ignored")
        message("   use `cmake -DBUILD_TYPE=Debug` to turn on logging")
    endif()
endif()
if (${LOG_TRACE} MATCHES "on")
    if (${BUILD_TYPE} MATCHES Debug)
        message("-- Trace log is enabled")
        add_definitions(-DLOG_TRACE)
    else()
        message("-- ** WARNING ** Trace log is enabled but ignored")
        message("   use `cmake -DBUILD_TYPE=Debug` to turn on logging")
    endif()
endif()
if (${LOG_DEBUG} MATCHES "on")
    if (${BUILD_TYPE} MATCHES Debug)
        message("-- Debug log is enabled")
        add_definitions(-DLOG_DEBUG)
    else()
        message("-- ** WARNING ** Debug log is enabled but ignored")
        message("   use `cmake -DBUILD_TYPE=Debug` to turn on logging")
    endif()
endif()
if (${LOG_WARN} MATCHES "on")
    if (${BUILD_TYPE} MATCHES Debug)
        message("-- Warn log is enabled")
        add_definitions(-DLOG_DEBUG)
    else()
        message("-- ** WARNING ** Warn log is enabled but ignored")
        message("   use `cmake -DBUILD_TYPE=Debug` to turn on logging")
    endif()
endif()

set (CMAKE_C_FLAGS                "-std=c11 -Wall -Wextra -Werror -Wno-nullability-completeness -Wno-expansion-to-defined")
set (CMAKE_C_FLAGS_DEBUG          "-g")
set (CMAKE_C_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
set (CMAKE_C_FLAGS_RELEASE        "-O3 -DNDEBUG")
set (CMAKE_C_FLAGS_RELWITHDEBINFO "-O2 -g")

set (CMAKE_CXX_FLAGS                "-std=c++11 -Wall -Wextra -Werror -Wno-nullability-completeness -Wno-expansion-to-defined")
set (CMAKE_CXX_FLAGS_DEBUG          "-g")
set (CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
set (CMAKE_CXX_FLAGS_RELEASE        "-O3 -DNDEBUG")
set (CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")

add_library(karbonit-sources STATIC ${LIB_SOURCES}) # used to share object files across different dependent targets
link_libraries(karbonit-sources ${LIBS})

add_subdirectory("${CMAKE_SOURCE_DIR}/tools")
add_subdirectory("${CMAKE_SOURCE_DIR}/examples")
add_subdirectory("${CMAKE_SOURCE_DIR}/bench")

enable_testing()
add_subdirectory("${CMAKE_SOURCE_DIR}/tests")


