#include <numeric>
#include <vector>

#include <gtest/gtest.h>

#include "thread_pool.h"

TEST(ThreadPoolTest, BasicAssertions) {
    EXPECT_STRNE("hello", "world");
    EXPECT_EQ(7 * 6, 42);
}

TEST(ThreadPoolTest, Enqueue) {
    ThreadPool pool(2);
    auto future = pool.Enqueue([](int x) { return x * 2; }, 10);
    EXPECT_EQ(future.get(), 20);
}

TEST(ThreadPoolTest, ForEach) {
    ThreadPool pool(4);
    std::vector<int> data(100);
    std::iota(data.begin(), data.end(), 0);

    pool.ForEach(data.begin(), data.end(), [](int& x) { x *= 2; });

    for (int i = 0; i < 100; ++i) {
        EXPECT_EQ(data[i], i * 2);
    }
}

TEST(ThreadPoolTest, TransformReduce) {
    ThreadPool pool(4);
    std::vector<int> data(100);
    std::iota(data.begin(), data.end(), 1);

    auto result = pool.TransformReduce(data.begin(), data.end(), 0,
        [](int x) { return x; },
        [](int a, int b) { return a + b; });

    EXPECT_EQ(result, 5050);
}