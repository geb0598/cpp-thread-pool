#==============================================================================
# CMake configuration for the ThreadPool project
#==============================================================================

# Set the minimum required version of CMake
cmake_minimum_required(VERSION 3.10)

# Set the project name
project(ThreadPool)

#==============================================================================
# Project-wide configurations
#==============================================================================

# Set the C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Specify the MSVC runtime library to avoid linkage errors with dependencies
if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# Add the 'include' directory to the project's include path
include_directories(include)

#==============================================================================
# Add subdirectories
#==============================================================================

# Add the source directory, which contains the thread_pool library
add_subdirectory(src)

# Enable testing for the project
enable_testing()

# Add the test directory, which contains tests for the thread pool
add_subdirectory(test)

# Add the demo directory, which contains the example program
add_subdirectory(demo)
