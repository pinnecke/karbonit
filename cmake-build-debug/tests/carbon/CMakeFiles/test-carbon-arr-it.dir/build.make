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
CMAKE_BINARY_DIR = /Users/marcus/git/karbonit/cmake-build-debug

# Include any dependencies generated for this target.
include tests/carbon/CMakeFiles/test-carbon-arr-it.dir/depend.make

# Include the progress variables for this target.
include tests/carbon/CMakeFiles/test-carbon-arr-it.dir/progress.make

# Include the compile flags for this target's objects.
include tests/carbon/CMakeFiles/test-carbon-arr-it.dir/flags.make

tests/carbon/CMakeFiles/test-carbon-arr-it.dir/test-carbon-arr-it.cpp.o: tests/carbon/CMakeFiles/test-carbon-arr-it.dir/flags.make
tests/carbon/CMakeFiles/test-carbon-arr-it.dir/test-carbon-arr-it.cpp.o: ../tests/carbon/test-carbon-arr-it.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/marcus/git/karbonit/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object tests/carbon/CMakeFiles/test-carbon-arr-it.dir/test-carbon-arr-it.cpp.o"
	cd /Users/marcus/git/karbonit/cmake-build-debug/tests/carbon && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/test-carbon-arr-it.dir/test-carbon-arr-it.cpp.o -c /Users/marcus/git/karbonit/tests/carbon/test-carbon-arr-it.cpp

tests/carbon/CMakeFiles/test-carbon-arr-it.dir/test-carbon-arr-it.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/test-carbon-arr-it.dir/test-carbon-arr-it.cpp.i"
	cd /Users/marcus/git/karbonit/cmake-build-debug/tests/carbon && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/marcus/git/karbonit/tests/carbon/test-carbon-arr-it.cpp > CMakeFiles/test-carbon-arr-it.dir/test-carbon-arr-it.cpp.i

tests/carbon/CMakeFiles/test-carbon-arr-it.dir/test-carbon-arr-it.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/test-carbon-arr-it.dir/test-carbon-arr-it.cpp.s"
	cd /Users/marcus/git/karbonit/cmake-build-debug/tests/carbon && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/marcus/git/karbonit/tests/carbon/test-carbon-arr-it.cpp -o CMakeFiles/test-carbon-arr-it.dir/test-carbon-arr-it.cpp.s

# Object files for target test-carbon-arr-it
test__carbon__arr__it_OBJECTS = \
"CMakeFiles/test-carbon-arr-it.dir/test-carbon-arr-it.cpp.o"

# External object files for target test-carbon-arr-it
test__carbon__arr__it_EXTERNAL_OBJECTS =

../bin/test-carbon-arr-it: tests/carbon/CMakeFiles/test-carbon-arr-it.dir/test-carbon-arr-it.cpp.o
../bin/test-carbon-arr-it: tests/carbon/CMakeFiles/test-carbon-arr-it.dir/build.make
../bin/test-carbon-arr-it: libjakson-sources.a
../bin/test-carbon-arr-it: libjakson-sources.a
../bin/test-carbon-arr-it: tests/carbon/CMakeFiles/test-carbon-arr-it.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/marcus/git/karbonit/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../../../bin/test-carbon-arr-it"
	cd /Users/marcus/git/karbonit/cmake-build-debug/tests/carbon && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/test-carbon-arr-it.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
tests/carbon/CMakeFiles/test-carbon-arr-it.dir/build: ../bin/test-carbon-arr-it

.PHONY : tests/carbon/CMakeFiles/test-carbon-arr-it.dir/build

tests/carbon/CMakeFiles/test-carbon-arr-it.dir/clean:
	cd /Users/marcus/git/karbonit/cmake-build-debug/tests/carbon && $(CMAKE_COMMAND) -P CMakeFiles/test-carbon-arr-it.dir/cmake_clean.cmake
.PHONY : tests/carbon/CMakeFiles/test-carbon-arr-it.dir/clean

tests/carbon/CMakeFiles/test-carbon-arr-it.dir/depend:
	cd /Users/marcus/git/karbonit/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/marcus/git/karbonit /Users/marcus/git/karbonit/tests/carbon /Users/marcus/git/karbonit/cmake-build-debug /Users/marcus/git/karbonit/cmake-build-debug/tests/carbon /Users/marcus/git/karbonit/cmake-build-debug/tests/carbon/CMakeFiles/test-carbon-arr-it.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : tests/carbon/CMakeFiles/test-carbon-arr-it.dir/depend

