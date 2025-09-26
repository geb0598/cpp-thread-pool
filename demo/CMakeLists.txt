#==============================================================================
# Build configuration for the demo program
#==============================================================================

# Set the minimum required version of CMake
cmake_minimum_required(VERSION 3.10)

# Set the project name for the demo
project(ThreadPoolDemo)

#==============================================================================
# Demo Executable
#==============================================================================

# Create the demo executable from demo.cpp
add_executable(thread_pool_demo demo.cpp)

# Link the demo executable against our thread_pool library.
# The include directories are handled automatically because we made them
# a PUBLIC property of the thread_pool target in src/CMakeLists.txt.
target_link_libraries(thread_pool_demo PRIVATE thread_pool)