# CMAKE generated file: DO NOT EDIT!
# Generated by "MSYS Makefiles" Generator, CMake Version 3.0

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
CMAKE_COMMAND = "/c/Program Files (x86)/CMake/bin/cmake.exe"

# The command to remove a file.
RM = "/c/Program Files (x86)/CMake/bin/cmake.exe" -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /c/Users/lasilva/Documents/GitHub/lincxx

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /C/Users/lasilva/Documents/GitHub/lincxx/build_llvm

# Include any dependencies generated for this target.
include test/gtest/CMakeFiles/gtest.dir/depend.make

# Include the progress variables for this target.
include test/gtest/CMakeFiles/gtest.dir/progress.make

# Include the compile flags for this target's objects.
include test/gtest/CMakeFiles/gtest.dir/flags.make

test/gtest/CMakeFiles/gtest.dir/src/gtest-all.cc.obj: test/gtest/CMakeFiles/gtest.dir/flags.make
test/gtest/CMakeFiles/gtest.dir/src/gtest-all.cc.obj: test/gtest/CMakeFiles/gtest.dir/includes_CXX.rsp
test/gtest/CMakeFiles/gtest.dir/src/gtest-all.cc.obj: ../test/gtest/src/gtest-all.cc
	$(CMAKE_COMMAND) -E cmake_progress_report /C/Users/lasilva/Documents/GitHub/lincxx/build_llvm/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object test/gtest/CMakeFiles/gtest.dir/src/gtest-all.cc.obj"
	cd /C/Users/lasilva/Documents/GitHub/lincxx/build_llvm/test/gtest && /d/toolchains/mingw/bin/clang++.exe   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/gtest.dir/src/gtest-all.cc.obj -c /c/Users/lasilva/Documents/GitHub/lincxx/test/gtest/src/gtest-all.cc

test/gtest/CMakeFiles/gtest.dir/src/gtest-all.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/gtest.dir/src/gtest-all.cc.i"
	cd /C/Users/lasilva/Documents/GitHub/lincxx/build_llvm/test/gtest && /d/toolchains/mingw/bin/clang++.exe  $(CXX_DEFINES) $(CXX_FLAGS) -E /c/Users/lasilva/Documents/GitHub/lincxx/test/gtest/src/gtest-all.cc > CMakeFiles/gtest.dir/src/gtest-all.cc.i

test/gtest/CMakeFiles/gtest.dir/src/gtest-all.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/gtest.dir/src/gtest-all.cc.s"
	cd /C/Users/lasilva/Documents/GitHub/lincxx/build_llvm/test/gtest && /d/toolchains/mingw/bin/clang++.exe  $(CXX_DEFINES) $(CXX_FLAGS) -S /c/Users/lasilva/Documents/GitHub/lincxx/test/gtest/src/gtest-all.cc -o CMakeFiles/gtest.dir/src/gtest-all.cc.s

test/gtest/CMakeFiles/gtest.dir/src/gtest-all.cc.obj.requires:
.PHONY : test/gtest/CMakeFiles/gtest.dir/src/gtest-all.cc.obj.requires

test/gtest/CMakeFiles/gtest.dir/src/gtest-all.cc.obj.provides: test/gtest/CMakeFiles/gtest.dir/src/gtest-all.cc.obj.requires
	$(MAKE) -f test/gtest/CMakeFiles/gtest.dir/build.make test/gtest/CMakeFiles/gtest.dir/src/gtest-all.cc.obj.provides.build
.PHONY : test/gtest/CMakeFiles/gtest.dir/src/gtest-all.cc.obj.provides

test/gtest/CMakeFiles/gtest.dir/src/gtest-all.cc.obj.provides.build: test/gtest/CMakeFiles/gtest.dir/src/gtest-all.cc.obj

# Object files for target gtest
gtest_OBJECTS = \
"CMakeFiles/gtest.dir/src/gtest-all.cc.obj"

# External object files for target gtest
gtest_EXTERNAL_OBJECTS =

test/gtest/libgtest.a: test/gtest/CMakeFiles/gtest.dir/src/gtest-all.cc.obj
test/gtest/libgtest.a: test/gtest/CMakeFiles/gtest.dir/build.make
test/gtest/libgtest.a: test/gtest/CMakeFiles/gtest.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX static library libgtest.a"
	cd /C/Users/lasilva/Documents/GitHub/lincxx/build_llvm/test/gtest && $(CMAKE_COMMAND) -P CMakeFiles/gtest.dir/cmake_clean_target.cmake
	cd /C/Users/lasilva/Documents/GitHub/lincxx/build_llvm/test/gtest && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/gtest.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
test/gtest/CMakeFiles/gtest.dir/build: test/gtest/libgtest.a
.PHONY : test/gtest/CMakeFiles/gtest.dir/build

test/gtest/CMakeFiles/gtest.dir/requires: test/gtest/CMakeFiles/gtest.dir/src/gtest-all.cc.obj.requires
.PHONY : test/gtest/CMakeFiles/gtest.dir/requires

test/gtest/CMakeFiles/gtest.dir/clean:
	cd /C/Users/lasilva/Documents/GitHub/lincxx/build_llvm/test/gtest && $(CMAKE_COMMAND) -P CMakeFiles/gtest.dir/cmake_clean.cmake
.PHONY : test/gtest/CMakeFiles/gtest.dir/clean

test/gtest/CMakeFiles/gtest.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MSYS Makefiles" /c/Users/lasilva/Documents/GitHub/lincxx /c/Users/lasilva/Documents/GitHub/lincxx/test/gtest /C/Users/lasilva/Documents/GitHub/lincxx/build_llvm /C/Users/lasilva/Documents/GitHub/lincxx/build_llvm/test/gtest /C/Users/lasilva/Documents/GitHub/lincxx/build_llvm/test/gtest/CMakeFiles/gtest.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : test/gtest/CMakeFiles/gtest.dir/depend

