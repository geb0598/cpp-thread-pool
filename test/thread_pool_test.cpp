#include <gtest/gtest.h>
#include "thread_pool.h"

#include <atomic>
#include <chrono>
#include <future>
#include <numeric>
#include <stdexcept>
#include <vector>
#include <thread> // For std::this_thread::sleep_for

// Helper function to simulate work
void simulate_work(int milliseconds) {
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

// Test fixture for ThreadPool
class ThreadPoolTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Common setup if needed
    }

    void TearDown() override {
        // Common teardown if needed
    }
};

//==============================================================================
// ThreadPool Construction Tests
//==============================================================================

TEST_F(ThreadPoolTest, DefaultConstructorUsesHardwareConcurrency) {
    // Just ensure it constructs without throwing
    ThreadPool pool;
    SUCCEED();
}

TEST_F(ThreadPoolTest, ConstructorWithZeroThreadsThrows) {
    ASSERT_THROW(ThreadPool(0), std::runtime_error);
}

//==============================================================================
// Enqueue Tests
//=============================================================================

TEST_F(ThreadPoolTest, EnqueueSimpleTask) {
    ThreadPool pool(1);
    auto future = pool.Enqueue([]() { return 42; });
    EXPECT_EQ(future.get(), 42);
}

TEST_F(ThreadPoolTest, EnqueueTaskWithArguments) {
    ThreadPool pool(1);
    auto future = pool.Enqueue([](int a, int b) { return a + b; }, 5, 7);
    EXPECT_EQ(future.get(), 12);
}

TEST_F(ThreadPoolTest, EnqueueMultipleTasks) {
    ThreadPool pool(4);
    std::vector<std::future<int>> futures;
    for (int i = 0; i < 100; ++i) {
        futures.push_back(pool.Enqueue([](int x) { return x * 2; }, i));
    }

    for (int i = 0; i < 100; ++i) {
        EXPECT_EQ(futures[i].get(), i * 2);
    }
}

TEST_F(ThreadPoolTest, EnqueueLongRunningTasks) {
    ThreadPool pool(2);
    auto future1 = pool.Enqueue([]() { simulate_work(200); return 1; });
    auto future2 = pool.Enqueue([]() { simulate_work(200); return 2; });
    auto future3 = pool.Enqueue([]() { simulate_work(200); return 3; }); // This should wait

    auto start = std::chrono::high_resolution_clock::now();
    future1.get();
    future2.get();
    future3.get();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = end - start;

    // Expect total time to be roughly 400ms (2 tasks in parallel, then 1)
    // Allow for some overhead
    EXPECT_GE(duration.count(), 350);
    EXPECT_LE(duration.count(), 700); // Upper bound to catch excessive delays
}

TEST_F(ThreadPoolTest, EnqueueTaskThatThrowsException) {
    ThreadPool pool(1);
    auto future = pool.Enqueue([]() -> int { throw std::runtime_error("Test Exception"); return 0; });
    ASSERT_THROW(future.get(), std::runtime_error);
}

//==============================================================================
// ForEach Tests
//==============================================================================

TEST_F(ThreadPoolTest, ForEachEmptyRange) {
    ThreadPool pool(2);
    std::vector<int> data;
    pool.ForEach(data.begin(), data.end(), [](int& x) { x = 1; }); // Should do nothing
    EXPECT_TRUE(data.empty());
}

TEST_F(ThreadPoolTest, ForEachSingleElementRange) {
    ThreadPool pool(1);
    std::vector<int> data = {10};
    pool.ForEach(data.begin(), data.end(), [](int& x) { x *= 2; });
    EXPECT_EQ(data[0], 20);
}

TEST_F(ThreadPoolTest, ForEachLargeRange) {
    ThreadPool pool(4);
    const size_t size = 10000;
    std::vector<int> data(size);
    std::iota(data.begin(), data.end(), 0);

    pool.ForEach(data.begin(), data.end(), [](int& x) { x *= 2; });

    for (size_t i = 0; i < size; ++i) {
        EXPECT_EQ(data[i], static_cast<int>(i * 2));
    }
}

TEST_F(ThreadPoolTest, ForEachWithSharedResource) {
    ThreadPool pool(4);
    std::atomic<int> counter = 0;
    std::vector<int> data(1000);
    // std::iota(data.begin(), data.end(), 0);

    pool.ForEach(data.begin(), data.end(), [&](int& x) {
        x = ++counter; // Atomically increment and assign
    });

    EXPECT_EQ(counter.load(), 1000);
    // Verify that all elements got a unique value (implies no race condition on counter)
    std::vector<int> sorted_data = data;
    std::sort(sorted_data.begin(), sorted_data.end());
    for (int i = 0; i < 1000; ++i) {
        EXPECT_EQ(sorted_data[i], i + 1);
    }
}

TEST_F(ThreadPoolTest, ForEachTaskThatThrowsException) {
    ThreadPool pool(2);
    std::vector<int> data = {1, 2, 3};
    // If any task throws, ForEach should propagate it
    ASSERT_THROW(
        pool.ForEach(data.begin(), data.end(), [](int& x) {
            if (x == 2) throw std::runtime_error("ForEach Test Exception");
            x *= 2;
        }),
        std::runtime_error
    );
    // Ensure some tasks might have completed before the exception
    EXPECT_TRUE(data[0] == 2 || data[0] == 1); // 1 might be doubled or not
}

//==============================================================================
// TransformReduce Tests
//==============================================================================

TEST_F(ThreadPoolTest, TransformReduceEmptyRange) {
    ThreadPool pool(2);
    std::vector<int> data;
    auto result = pool.TransformReduce(data.begin(), data.end(), 0,
        [](int x) { return x * 2; },
        std::plus<int>());
    EXPECT_EQ(result, 0);
}

TEST_F(ThreadPoolTest, TransformReduceSingleElementRange) {
    ThreadPool pool(1);
    std::vector<int> data = {10};
    auto result = pool.TransformReduce(data.begin(), data.end(), 0,
        [](int x) { return x * 2; },
        std::plus<int>());
    EXPECT_EQ(result, 20);
}

TEST_F(ThreadPoolTest, TransformReduceSumOfSquares) {
    ThreadPool pool(4);
    const size_t size = 100;
    std::vector<int> data(size);
    std::iota(data.begin(), data.end(), 1); // 1 to 100

    auto result = pool.TransformReduce(data.begin(), data.end(), 0,
        [](int x) { return x * x; },
        std::plus<int>());

    // Sum of squares from 1 to 100 is 100 * (100 + 1) * (2 * 100 + 1) / 6 = 338350
    EXPECT_EQ(result, 338350);
}

TEST_F(ThreadPoolTest, TransformReduceProduct) {
    ThreadPool pool(4);
    std::vector<int> data = {1, 2, 3, 4, 5};
    auto result = pool.TransformReduce(data.begin(), data.end(), 1, // Initial value for product is 1
        [](int x) { return x; },
        std::multiplies<int>());
    EXPECT_EQ(result, 120); // 1 * 2 * 3 * 4 * 5
}

TEST_F(ThreadPoolTest, TransformReduceWithCustomTypes) {
    ThreadPool pool(2);
    std::vector<std::string> words = {"hello", "world", "cpp"};
    std::string combined = pool.TransformReduce(words.begin(), words.end(), std::string(""),
        [](const std::string& s) { return s + " "; }, // Add space after each word
        [](std::string acc, std::string val) { return acc + val; });
    EXPECT_EQ(combined, "hello world cpp ");
}

TEST_F(ThreadPoolTest, TransformReduceTaskThatThrowsException) {
    ThreadPool pool(2);
    std::vector<int> data = {1, 2, 3};
    ASSERT_THROW(
        pool.TransformReduce(data.begin(), data.end(), 0,
            [](int x) -> int {
                if (x == 2) throw std::runtime_error("Transform Test Exception");
                return x * 2;
            },
            std::plus<int>()),
        std::runtime_error
    );
}

//==============================================================================
// ThreadPool Shutdown Tests
//==============================================================================

TEST_F(ThreadPoolTest, DestructorWaitsForAllTasks) {
    std::atomic<int> completed_tasks = 0;
    {
        ThreadPool pool(2);
        for (int i = 0; i < 5; ++i) {
            pool.Enqueue([&]() {
                simulate_work(50); // Simulate some work
                completed_tasks++;
            });
        }
    } // Pool destructor called here

    EXPECT_EQ(completed_tasks.load(), 5); // All tasks should have completed
}

TEST_F(ThreadPoolTest, DestructorHandlesNoTasks) {
    // Just ensure the destructor doesn't crash or hang with an empty pool
    {
        ThreadPool pool(2);
    }
    SUCCEED();
}

TEST_F(ThreadPoolTest, EnqueueTaskThatEnqueuesMoreTasks) {
    ThreadPool pool(2);
    std::atomic<int> counter = 0;
    std::promise<void> promise;
    auto future = promise.get_future();

    pool.Enqueue([&]() {
        counter++;
        pool.Enqueue([&]() {
            counter++;
            promise.set_value();
        });
    });

    future.wait();
    EXPECT_EQ(counter.load(), 2);
}

TEST_F(ThreadPoolTest, DestructorHandlesPendingTasks) {
    std::atomic<int> completed_tasks = 0;
    {
        ThreadPool pool(1);
        // Block the only thread
        pool.Enqueue([&]() {
            simulate_work(100);
            completed_tasks++;
        });

        // Enqueue more tasks that will be pending
        for (int i = 0; i < 3; ++i) {
            pool.Enqueue([&]() {
                completed_tasks++;
            });
        }
    } // Destructor is called here, should wait for all 4 tasks

    EXPECT_EQ(completed_tasks.load(), 4);
}

TEST_F(ThreadPoolTest, StressTestWithManyShortTasks) {
    ThreadPool pool(std::thread::hardware_concurrency());
    const int num_tasks = 10000;
    std::atomic<int> counter = 0;

    {
        std::vector<std::future<void>> futures;
        for (int i = 0; i < num_tasks; ++i) {
            futures.push_back(pool.Enqueue([&]() {
                counter++;
            }));
        }

        for(auto& f : futures) {
            f.get();
        }
    }

    EXPECT_EQ(counter.load(), num_tasks);
}

TEST_F(ThreadPoolTest, CorrectlyCalculatesPartialSum) {
    const size_t num_threads = 8;
    ThreadPool pool(num_threads);
    const size_t vector_size = 10000000;
    const int chunk_size = vector_size / num_threads;
    std::vector<long long> numbers(vector_size);
    std::iota(numbers.begin(), numbers.end(), 1);

    std::vector<std::future<long long>> futures;
    for (size_t i = 0; i < num_threads; ++i) {
        futures.push_back(pool.Enqueue([&numbers, i, chunk_size, num_threads]() {
            long long partial_sum = 0;
            size_t start = i * chunk_size;
            size_t end = (i == num_threads - 1) ? numbers.size() : (i + 1) * chunk_size;
            for (size_t j = start; j < end; ++j) {
                partial_sum += numbers[j];
            }
            return partial_sum;
        }));
    }

    long long total_sum = 0;
    for (auto& future : futures) {
        total_sum += future.get();
    }

    long long expected_sum = (static_cast<long long>(vector_size) * (vector_size + 1)) / 2;
    EXPECT_EQ(total_sum, expected_sum);
}
