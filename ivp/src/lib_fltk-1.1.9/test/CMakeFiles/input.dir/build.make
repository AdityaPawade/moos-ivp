# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.6

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canoncical targets will work.
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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# The program to use to edit the cache.
CMAKE_EDIT_COMMAND = /usr/bin/ccmake

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/cjc/moos-ivp/ivp/src

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/cjc/moos-ivp/ivp/src

# Include any dependencies generated for this target.
include lib_fltk-1.1.9/test/CMakeFiles/input.dir/depend.make

# Include the progress variables for this target.
include lib_fltk-1.1.9/test/CMakeFiles/input.dir/progress.make

# Include the compile flags for this target's objects.
include lib_fltk-1.1.9/test/CMakeFiles/input.dir/flags.make

lib_fltk-1.1.9/test/CMakeFiles/input.dir/input.o: lib_fltk-1.1.9/test/CMakeFiles/input.dir/flags.make
lib_fltk-1.1.9/test/CMakeFiles/input.dir/input.o: lib_fltk-1.1.9/test/input.cxx
	$(CMAKE_COMMAND) -E cmake_progress_report /home/cjc/moos-ivp/ivp/src/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object lib_fltk-1.1.9/test/CMakeFiles/input.dir/input.o"
	cd /home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/test && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/input.dir/input.o -c /home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/test/input.cxx

lib_fltk-1.1.9/test/CMakeFiles/input.dir/input.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/input.dir/input.i"
	cd /home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/test && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/test/input.cxx > CMakeFiles/input.dir/input.i

lib_fltk-1.1.9/test/CMakeFiles/input.dir/input.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/input.dir/input.s"
	cd /home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/test && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/test/input.cxx -o CMakeFiles/input.dir/input.s

lib_fltk-1.1.9/test/CMakeFiles/input.dir/input.o.requires:
.PHONY : lib_fltk-1.1.9/test/CMakeFiles/input.dir/input.o.requires

lib_fltk-1.1.9/test/CMakeFiles/input.dir/input.o.provides: lib_fltk-1.1.9/test/CMakeFiles/input.dir/input.o.requires
	$(MAKE) -f lib_fltk-1.1.9/test/CMakeFiles/input.dir/build.make lib_fltk-1.1.9/test/CMakeFiles/input.dir/input.o.provides.build
.PHONY : lib_fltk-1.1.9/test/CMakeFiles/input.dir/input.o.provides

lib_fltk-1.1.9/test/CMakeFiles/input.dir/input.o.provides.build: lib_fltk-1.1.9/test/CMakeFiles/input.dir/input.o
.PHONY : lib_fltk-1.1.9/test/CMakeFiles/input.dir/input.o.provides.build

# Object files for target input
input_OBJECTS = \
"CMakeFiles/input.dir/input.o"

# External object files for target input
input_EXTERNAL_OBJECTS =

bin/input: lib_fltk-1.1.9/test/CMakeFiles/input.dir/input.o
bin/input: lib_fltk-1.1.9/src/libfltk.a
bin/input: /usr/lib/libSM.so
bin/input: /usr/lib/libICE.so
bin/input: /usr/lib/libX11.so
bin/input: /usr/lib/libXext.so
bin/input: /usr/lib/libSM.so
bin/input: /usr/lib/libICE.so
bin/input: /usr/lib/libX11.so
bin/input: /usr/lib/libXext.so
bin/input: lib_fltk-1.1.9/test/CMakeFiles/input.dir/build.make
bin/input: lib_fltk-1.1.9/test/CMakeFiles/input.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable ../../bin/input"
	cd /home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/test && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/input.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
lib_fltk-1.1.9/test/CMakeFiles/input.dir/build: bin/input
.PHONY : lib_fltk-1.1.9/test/CMakeFiles/input.dir/build

lib_fltk-1.1.9/test/CMakeFiles/input.dir/requires: lib_fltk-1.1.9/test/CMakeFiles/input.dir/input.o.requires
.PHONY : lib_fltk-1.1.9/test/CMakeFiles/input.dir/requires

lib_fltk-1.1.9/test/CMakeFiles/input.dir/clean:
	cd /home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/test && $(CMAKE_COMMAND) -P CMakeFiles/input.dir/cmake_clean.cmake
.PHONY : lib_fltk-1.1.9/test/CMakeFiles/input.dir/clean

lib_fltk-1.1.9/test/CMakeFiles/input.dir/depend:
	cd /home/cjc/moos-ivp/ivp/src && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/cjc/moos-ivp/ivp/src /home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/test /home/cjc/moos-ivp/ivp/src /home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/test /home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/test/CMakeFiles/input.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : lib_fltk-1.1.9/test/CMakeFiles/input.dir/depend

