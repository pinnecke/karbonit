# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.15

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake

# The command to remove a file.
RM = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/marcus/git/karbonit

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/marcus/git/karbonit

# Include any dependencies generated for this target.
include tests/misc/CMakeFiles/test-thread-pool.dir/depend.make

# Include the progress variables for this target.
include tests/misc/CMakeFiles/test-thread-pool.dir/progress.make

# Include the compile flags for this target's objects.
include tests/misc/CMakeFiles/test-thread-pool.dir/flags.make

tests/misc/CMakeFiles/test-thread-pool.dir/test-thread-pool.cpp.o: tests/misc/CMakeFiles/test-thread-pool.dir/flags.make
tests/misc/CMakeFiles/test-thread-pool.dir/test-thread-pool.cpp.o: tests/misc/test-thread-pool.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/marcus/git/karbonit/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object tests/misc/CMakeFiles/test-thread-pool.dir/test-thread-pool.cpp.o"
	cd /Users/marcus/git/karbonit/tests/misc && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/test-thread-pool.dir/test-thread-pool.cpp.o -c /Users/marcus/git/karbonit/tests/misc/test-thread-pool.cpp

tests/misc/CMakeFiles/test-thread-pool.dir/test-thread-pool.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/test-thread-pool.dir/test-thread-pool.cpp.i"
	cd /Users/marcus/git/karbonit/tests/misc && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/marcus/git/karbonit/tests/misc/test-thread-pool.cpp > CMakeFiles/test-thread-pool.dir/test-thread-pool.cpp.i

tests/misc/CMakeFiles/test-thread-pool.dir/test-thread-pool.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/test-thread-pool.dir/test-thread-pool.cpp.s"
	cd /Users/marcus/git/karbonit/tests/misc && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/marcus/git/karbonit/tests/misc/test-thread-pool.cpp -o CMakeFiles/test-thread-pool.dir/test-thread-pool.cpp.s

# Object files for target test-thread-pool
test__thread__pool_OBJECTS = \
"CMakeFiles/test-thread-pool.dir/test-thread-pool.cpp.o"

# External object files for target test-thread-pool
test__thread__pool_EXTERNAL_OBJECTS =

bin/test-thread-pool: tests/misc/CMakeFiles/test-thread-pool.dir/test-thread-pool.cpp.o
bin/test-thread-pool: tests/misc/CMakeFiles/test-thread-pool.dir/build.make
bin/test-thread-pool: libkarbonit-sources.a
bin/test-thread-pool: libkarbonit-sources.a
bin/test-thread-pool: tests/misc/CMakeFiles/test-thread-pool.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/marcus/git/karbonit/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../../bin/test-thread-pool"
	cd /Users/marcus/git/karbonit/tests/misc && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/test-thread-pool.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
tests/misc/CMakeFiles/test-thread-pool.dir/build: bin/test-thread-pool

.PHONY : tests/misc/CMakeFiles/test-thread-pool.dir/build

tests/misc/CMakeFiles/test-thread-pool.dir/clean:
	cd /Users/marcus/git/karbonit/tests/misc && $(CMAKE_COMMAND) -P CMakeFiles/test-thread-pool.dir/cmake_clean.cmake
.PHONY : tests/misc/CMakeFiles/test-thread-pool.dir/clean

tests/misc/CMakeFiles/test-thread-pool.dir/depend:
	cd /Users/marcus/git/karbonit && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/marcus/git/karbonit /Users/marcus/git/karbonit/tests/misc /Users/marcus/git/karbonit /Users/marcus/git/karbonit/tests/misc /Users/marcus/git/karbonit/tests/misc/CMakeFiles/test-thread-pool.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : tests/misc/CMakeFiles/test-thread-pool.dir/depend

