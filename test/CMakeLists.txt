include(FetchContent)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)

FetchContent_MakeAvailable(googletest)

add_executable(thread_pool_test
    thread_pool_test.cpp
)

target_link_libraries(thread_pool_test
    thread_pool
    gtest
    gtest_main
)

include(GoogleTest)
gtest_discover_tests(thread_pool_test)