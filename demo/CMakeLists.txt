cmake_minimum_required(VERSION 3.10)

project(ThreadPoolDemo)

add_executable(thread_pool_demo demo.cpp)

# Add the include directory for thread_pool.h
target_include_directories(thread_pool_demo PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Link against the thread_pool library
target_link_libraries(thread_pool_demo PRIVATE thread_pool)
