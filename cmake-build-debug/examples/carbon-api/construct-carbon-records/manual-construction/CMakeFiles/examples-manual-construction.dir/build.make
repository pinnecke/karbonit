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
include examples/carbon-api/construct-carbon-records/manual-construction/CMakeFiles/examples-manual-construction.dir/depend.make

# Include the progress variables for this target.
include examples/carbon-api/construct-carbon-records/manual-construction/CMakeFiles/examples-manual-construction.dir/progress.make

# Include the compile flags for this target's objects.
include examples/carbon-api/construct-carbon-records/manual-construction/CMakeFiles/examples-manual-construction.dir/flags.make

examples/carbon-api/construct-carbon-records/manual-construction/CMakeFiles/examples-manual-construction.dir/manual-construction.c.o: examples/carbon-api/construct-carbon-records/manual-construction/CMakeFiles/examples-manual-construction.dir/flags.make
examples/carbon-api/construct-carbon-records/manual-construction/CMakeFiles/examples-manual-construction.dir/manual-construction.c.o: ../examples/carbon-api/construct-carbon-records/manual-construction/manual-construction.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/marcus/git/karbonit/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object examples/carbon-api/construct-carbon-records/manual-construction/CMakeFiles/examples-manual-construction.dir/manual-construction.c.o"
	cd /Users/marcus/git/karbonit/cmake-build-debug/examples/carbon-api/construct-carbon-records/manual-construction && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/examples-manual-construction.dir/manual-construction.c.o   -c /Users/marcus/git/karbonit/examples/carbon-api/construct-carbon-records/manual-construction/manual-construction.c

examples/carbon-api/construct-carbon-records/manual-construction/CMakeFiles/examples-manual-construction.dir/manual-construction.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/examples-manual-construction.dir/manual-construction.c.i"
	cd /Users/marcus/git/karbonit/cmake-build-debug/examples/carbon-api/construct-carbon-records/manual-construction && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/marcus/git/karbonit/examples/carbon-api/construct-carbon-records/manual-construction/manual-construction.c > CMakeFiles/examples-manual-construction.dir/manual-construction.c.i

examples/carbon-api/construct-carbon-records/manual-construction/CMakeFiles/examples-manual-construction.dir/manual-construction.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/examples-manual-construction.dir/manual-construction.c.s"
	cd /Users/marcus/git/karbonit/cmake-build-debug/examples/carbon-api/construct-carbon-records/manual-construction && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/marcus/git/karbonit/examples/carbon-api/construct-carbon-records/manual-construction/manual-construction.c -o CMakeFiles/examples-manual-construction.dir/manual-construction.c.s

# Object files for target examples-manual-construction
examples__manual__construction_OBJECTS = \
"CMakeFiles/examples-manual-construction.dir/manual-construction.c.o"

# External object files for target examples-manual-construction
examples__manual__construction_EXTERNAL_OBJECTS =

../bin/examples-manual-construction: examples/carbon-api/construct-carbon-records/manual-construction/CMakeFiles/examples-manual-construction.dir/manual-construction.c.o
../bin/examples-manual-construction: examples/carbon-api/construct-carbon-records/manual-construction/CMakeFiles/examples-manual-construction.dir/build.make
../bin/examples-manual-construction: libjakson-sources.a
../bin/examples-manual-construction: libjakson-sources.a
../bin/examples-manual-construction: examples/carbon-api/construct-carbon-records/manual-construction/CMakeFiles/examples-manual-construction.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/marcus/git/karbonit/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable ../../../../../bin/examples-manual-construction"
	cd /Users/marcus/git/karbonit/cmake-build-debug/examples/carbon-api/construct-carbon-records/manual-construction && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/examples-manual-construction.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
examples/carbon-api/construct-carbon-records/manual-construction/CMakeFiles/examples-manual-construction.dir/build: ../bin/examples-manual-construction

.PHONY : examples/carbon-api/construct-carbon-records/manual-construction/CMakeFiles/examples-manual-construction.dir/build

examples/carbon-api/construct-carbon-records/manual-construction/CMakeFiles/examples-manual-construction.dir/clean:
	cd /Users/marcus/git/karbonit/cmake-build-debug/examples/carbon-api/construct-carbon-records/manual-construction && $(CMAKE_COMMAND) -P CMakeFiles/examples-manual-construction.dir/cmake_clean.cmake
.PHONY : examples/carbon-api/construct-carbon-records/manual-construction/CMakeFiles/examples-manual-construction.dir/clean

examples/carbon-api/construct-carbon-records/manual-construction/CMakeFiles/examples-manual-construction.dir/depend:
	cd /Users/marcus/git/karbonit/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/marcus/git/karbonit /Users/marcus/git/karbonit/examples/carbon-api/construct-carbon-records/manual-construction /Users/marcus/git/karbonit/cmake-build-debug /Users/marcus/git/karbonit/cmake-build-debug/examples/carbon-api/construct-carbon-records/manual-construction /Users/marcus/git/karbonit/cmake-build-debug/examples/carbon-api/construct-carbon-records/manual-construction/CMakeFiles/examples-manual-construction.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : examples/carbon-api/construct-carbon-records/manual-construction/CMakeFiles/examples-manual-construction.dir/depend

