# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.17

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Disable VCS-based implicit rules.
% : %,v


# Disable VCS-based implicit rules.
% : RCS/%


# Disable VCS-based implicit rules.
% : RCS/%,v


# Disable VCS-based implicit rules.
% : SCCS/s.%


# Disable VCS-based implicit rules.
% : s.%


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
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/netpipe/Desktop/ELFTools

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/netpipe/Desktop/ELFTools

# Include any dependencies generated for this target.
include src/elf/CMakeFiles/elf.dir/depend.make

# Include the progress variables for this target.
include src/elf/CMakeFiles/elf.dir/progress.make

# Include the compile flags for this target's objects.
include src/elf/CMakeFiles/elf.dir/flags.make

src/elf/CMakeFiles/elf.dir/defs.cpp.o: src/elf/CMakeFiles/elf.dir/flags.make
src/elf/CMakeFiles/elf.dir/defs.cpp.o: src/elf/defs.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/netpipe/Desktop/ELFTools/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/elf/CMakeFiles/elf.dir/defs.cpp.o"
	cd /home/netpipe/Desktop/ELFTools/src/elf && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/elf.dir/defs.cpp.o -c /home/netpipe/Desktop/ELFTools/src/elf/defs.cpp

src/elf/CMakeFiles/elf.dir/defs.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/elf.dir/defs.cpp.i"
	cd /home/netpipe/Desktop/ELFTools/src/elf && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/netpipe/Desktop/ELFTools/src/elf/defs.cpp > CMakeFiles/elf.dir/defs.cpp.i

src/elf/CMakeFiles/elf.dir/defs.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/elf.dir/defs.cpp.s"
	cd /home/netpipe/Desktop/ELFTools/src/elf && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/netpipe/Desktop/ELFTools/src/elf/defs.cpp -o CMakeFiles/elf.dir/defs.cpp.s

src/elf/CMakeFiles/elf.dir/elf32.cpp.o: src/elf/CMakeFiles/elf.dir/flags.make
src/elf/CMakeFiles/elf.dir/elf32.cpp.o: src/elf/elf32.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/netpipe/Desktop/ELFTools/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object src/elf/CMakeFiles/elf.dir/elf32.cpp.o"
	cd /home/netpipe/Desktop/ELFTools/src/elf && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/elf.dir/elf32.cpp.o -c /home/netpipe/Desktop/ELFTools/src/elf/elf32.cpp

src/elf/CMakeFiles/elf.dir/elf32.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/elf.dir/elf32.cpp.i"
	cd /home/netpipe/Desktop/ELFTools/src/elf && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/netpipe/Desktop/ELFTools/src/elf/elf32.cpp > CMakeFiles/elf.dir/elf32.cpp.i

src/elf/CMakeFiles/elf.dir/elf32.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/elf.dir/elf32.cpp.s"
	cd /home/netpipe/Desktop/ELFTools/src/elf && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/netpipe/Desktop/ELFTools/src/elf/elf32.cpp -o CMakeFiles/elf.dir/elf32.cpp.s

src/elf/CMakeFiles/elf.dir/elf64.cpp.o: src/elf/CMakeFiles/elf.dir/flags.make
src/elf/CMakeFiles/elf.dir/elf64.cpp.o: src/elf/elf64.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/netpipe/Desktop/ELFTools/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object src/elf/CMakeFiles/elf.dir/elf64.cpp.o"
	cd /home/netpipe/Desktop/ELFTools/src/elf && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/elf.dir/elf64.cpp.o -c /home/netpipe/Desktop/ELFTools/src/elf/elf64.cpp

src/elf/CMakeFiles/elf.dir/elf64.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/elf.dir/elf64.cpp.i"
	cd /home/netpipe/Desktop/ELFTools/src/elf && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/netpipe/Desktop/ELFTools/src/elf/elf64.cpp > CMakeFiles/elf.dir/elf64.cpp.i

src/elf/CMakeFiles/elf.dir/elf64.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/elf.dir/elf64.cpp.s"
	cd /home/netpipe/Desktop/ELFTools/src/elf && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/netpipe/Desktop/ELFTools/src/elf/elf64.cpp -o CMakeFiles/elf.dir/elf64.cpp.s

src/elf/CMakeFiles/elf.dir/parser.cpp.o: src/elf/CMakeFiles/elf.dir/flags.make
src/elf/CMakeFiles/elf.dir/parser.cpp.o: src/elf/parser.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/netpipe/Desktop/ELFTools/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object src/elf/CMakeFiles/elf.dir/parser.cpp.o"
	cd /home/netpipe/Desktop/ELFTools/src/elf && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/elf.dir/parser.cpp.o -c /home/netpipe/Desktop/ELFTools/src/elf/parser.cpp

src/elf/CMakeFiles/elf.dir/parser.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/elf.dir/parser.cpp.i"
	cd /home/netpipe/Desktop/ELFTools/src/elf && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/netpipe/Desktop/ELFTools/src/elf/parser.cpp > CMakeFiles/elf.dir/parser.cpp.i

src/elf/CMakeFiles/elf.dir/parser.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/elf.dir/parser.cpp.s"
	cd /home/netpipe/Desktop/ELFTools/src/elf && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/netpipe/Desktop/ELFTools/src/elf/parser.cpp -o CMakeFiles/elf.dir/parser.cpp.s

elf: src/elf/CMakeFiles/elf.dir/defs.cpp.o
elf: src/elf/CMakeFiles/elf.dir/elf32.cpp.o
elf: src/elf/CMakeFiles/elf.dir/elf64.cpp.o
elf: src/elf/CMakeFiles/elf.dir/parser.cpp.o
elf: src/elf/CMakeFiles/elf.dir/build.make

.PHONY : elf

# Rule to build all files generated by this target.
src/elf/CMakeFiles/elf.dir/build: elf

.PHONY : src/elf/CMakeFiles/elf.dir/build

src/elf/CMakeFiles/elf.dir/clean:
	cd /home/netpipe/Desktop/ELFTools/src/elf && $(CMAKE_COMMAND) -P CMakeFiles/elf.dir/cmake_clean.cmake
.PHONY : src/elf/CMakeFiles/elf.dir/clean

src/elf/CMakeFiles/elf.dir/depend:
	cd /home/netpipe/Desktop/ELFTools && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/netpipe/Desktop/ELFTools /home/netpipe/Desktop/ELFTools/src/elf /home/netpipe/Desktop/ELFTools /home/netpipe/Desktop/ELFTools/src/elf /home/netpipe/Desktop/ELFTools/src/elf/CMakeFiles/elf.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/elf/CMakeFiles/elf.dir/depend

