#==============================================================================
# Build configuration for the tests
#==============================================================================

# Include the FetchContent module to download dependencies
include(FetchContent)

#==============================================================================
# Dependency: GoogleTest
#==============================================================================

# Force GoogleTest to use the same CRT (C Run-Time) as the main project.
# This is important on Windows with MSVC to avoid linker errors.
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Declare the GoogleTest dependency from its GitHub repository
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)

# Make the dependency available to the project.
# This will download and configure GoogleTest.
FetchContent_MakeAvailable(googletest)

#==============================================================================
# Test Executable
#==============================================================================

# Create the test executable
add_executable(thread_pool_test
    thread_pool_test.cpp
)

# Link the test executable against our thread_pool library and GoogleTest
target_link_libraries(thread_pool_test PRIVATE
    thread_pool
    gtest
    gtest_main
)

#==============================================================================
# Test Discovery
#==============================================================================

# Include the GoogleTest module to integrate with CTest
include(GoogleTest)

# Discover all tests in the executable so they can be run by CTest
gtest_discover_tests(thread_pool_test)
