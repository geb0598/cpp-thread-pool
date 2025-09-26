#==============================================================================
# Build configuration for the thread_pool library
#==============================================================================

# Add the thread_pool library as a STATIC library.
# A static library is linked at compile time.
add_library(thread_pool STATIC
    thread_pool.cpp
)

# Make the 'include' directory available to any target that links against this library.
# This ensures that consumers of this library can find 'thread_pool.h'.
target_include_directories(thread_pool PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)