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
include lib_fltk-1.1.9/test/CMakeFiles/subwindow.dir/depend.make

# Include the progress variables for this target.
include lib_fltk-1.1.9/test/CMakeFiles/subwindow.dir/progress.make

# Include the compile flags for this target's objects.
include lib_fltk-1.1.9/test/CMakeFiles/subwindow.dir/flags.make

lib_fltk-1.1.9/test/CMakeFiles/subwindow.dir/subwindow.o: lib_fltk-1.1.9/test/CMakeFiles/subwindow.dir/flags.make
lib_fltk-1.1.9/test/CMakeFiles/subwindow.dir/subwindow.o: lib_fltk-1.1.9/test/subwindow.cxx
	$(CMAKE_COMMAND) -E cmake_progress_report /home/cjc/moos-ivp/ivp/src/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object lib_fltk-1.1.9/test/CMakeFiles/subwindow.dir/subwindow.o"
	cd /home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/test && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/subwindow.dir/subwindow.o -c /home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/test/subwindow.cxx

lib_fltk-1.1.9/test/CMakeFiles/subwindow.dir/subwindow.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/subwindow.dir/subwindow.i"
	cd /home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/test && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/test/subwindow.cxx > CMakeFiles/subwindow.dir/subwindow.i

lib_fltk-1.1.9/test/CMakeFiles/subwindow.dir/subwindow.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/subwindow.dir/subwindow.s"
	cd /home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/test && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/test/subwindow.cxx -o CMakeFiles/subwindow.dir/subwindow.s

lib_fltk-1.1.9/test/CMakeFiles/subwindow.dir/subwindow.o.requires:
.PHONY : lib_fltk-1.1.9/test/CMakeFiles/subwindow.dir/subwindow.o.requires

lib_fltk-1.1.9/test/CMakeFiles/subwindow.dir/subwindow.o.provides: lib_fltk-1.1.9/test/CMakeFiles/subwindow.dir/subwindow.o.requires
	$(MAKE) -f lib_fltk-1.1.9/test/CMakeFiles/subwindow.dir/build.make lib_fltk-1.1.9/test/CMakeFiles/subwindow.dir/subwindow.o.provides.build
.PHONY : lib_fltk-1.1.9/test/CMakeFiles/subwindow.dir/subwindow.o.provides

lib_fltk-1.1.9/test/CMakeFiles/subwindow.dir/subwindow.o.provides.build: lib_fltk-1.1.9/test/CMakeFiles/subwindow.dir/subwindow.o
.PHONY : lib_fltk-1.1.9/test/CMakeFiles/subwindow.dir/subwindow.o.provides.build

# Object files for target subwindow
subwindow_OBJECTS = \
"CMakeFiles/subwindow.dir/subwindow.o"

# External object files for target subwindow
subwindow_EXTERNAL_OBJECTS =

bin/subwindow: lib_fltk-1.1.9/test/CMakeFiles/subwindow.dir/subwindow.o
bin/subwindow: lib_fltk-1.1.9/src/libfltk.a
bin/subwindow: /usr/lib/libSM.so
bin/subwindow: /usr/lib/libICE.so
bin/subwindow: /usr/lib/libX11.so
bin/subwindow: /usr/lib/libXext.so
bin/subwindow: /usr/lib/libSM.so
bin/subwindow: /usr/lib/libICE.so
bin/subwindow: /usr/lib/libX11.so
bin/subwindow: /usr/lib/libXext.so
bin/subwindow: lib_fltk-1.1.9/test/CMakeFiles/subwindow.dir/build.make
bin/subwindow: lib_fltk-1.1.9/test/CMakeFiles/subwindow.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable ../../bin/subwindow"
	cd /home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/test && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/subwindow.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
lib_fltk-1.1.9/test/CMakeFiles/subwindow.dir/build: bin/subwindow
.PHONY : lib_fltk-1.1.9/test/CMakeFiles/subwindow.dir/build

lib_fltk-1.1.9/test/CMakeFiles/subwindow.dir/requires: lib_fltk-1.1.9/test/CMakeFiles/subwindow.dir/subwindow.o.requires
.PHONY : lib_fltk-1.1.9/test/CMakeFiles/subwindow.dir/requires

lib_fltk-1.1.9/test/CMakeFiles/subwindow.dir/clean:
	cd /home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/test && $(CMAKE_COMMAND) -P CMakeFiles/subwindow.dir/cmake_clean.cmake
.PHONY : lib_fltk-1.1.9/test/CMakeFiles/subwindow.dir/clean

lib_fltk-1.1.9/test/CMakeFiles/subwindow.dir/depend:
	cd /home/cjc/moos-ivp/ivp/src && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/cjc/moos-ivp/ivp/src /home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/test /home/cjc/moos-ivp/ivp/src /home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/test /home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/test/CMakeFiles/subwindow.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : lib_fltk-1.1.9/test/CMakeFiles/subwindow.dir/depend

