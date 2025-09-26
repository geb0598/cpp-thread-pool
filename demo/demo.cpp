#include <iostream>
#include <vector>
#include <numeric>
#include <chrono>
#include <cmath>
#include <iomanip>
#include "thread_pool.h"

// A computationally intensive function to simulate work.
double heavy_computation(double value) {
    return std::sin(value) * std::cos(value) + std::pow(std::log(value + 1.0), 2.0);
}

void feature_demonstration() {
    std::cout << "--- Feature Demonstration ---" << std::endl;
    ThreadPool pool;

    // 1. Enqueue: Basic task that returns a value.
    std::cout << "Enqueueing a simple task..." << std::endl;
    auto future = pool.Enqueue([]() { 
        // No std::cout here to keep the main thread's output clean.
        return 42; 
    });
    std::cout << "Task returned: " << future.get() << std::endl;

    // 2. ForEach: Apply an in-place modification to a vector.
    std::cout << "Using ForEach to double each element in a vector..." << std::endl;
    std::vector<int> numbers = {1, 2, 3, 4, 5};
    std::cout << "Original vector: 1 2 3 4 5" << std::endl;
    pool.ForEach(numbers.begin(), numbers.end(), [](int& n) { n *= 2; });
    
    std::cout << "Modified vector: ";
    for (size_t i = 0; i < numbers.size(); ++i) {
        std::cout << numbers[i] << (i == numbers.size() - 1 ? "" : " ");
    }
    std::cout << std::endl;

    // 3. TransformReduce: Parallel computation and reduction.
    std::cout << "Using TransformReduce to compute sum of squares..." << std::endl;
    int sum_of_squares = pool.TransformReduce(numbers.begin(), numbers.end(), 0,
        [](int n) { return n * n; },
        std::plus<int>()
    );
    std::cout << "Sum of squares (on modified vector): " << sum_of_squares << std::endl;
}

void performance_comparison() {
    std::cout << "--- Performance Comparison ---" << std::endl;

    const size_t data_size = 100'000'000;
    std::vector<double> data(data_size);
    std::iota(data.begin(), data.end(), 1.0); // Fill with 1.0, 2.0, ...

    std::cout << "Performing heavy computation on " << data_size << " elements." << std::endl;

    // --- Single-threaded execution ---
    std::cout << "Running single-threaded computation..." << std::endl;
    auto start_single = std::chrono::high_resolution_clock::now();
    std::vector<double> result_single(data_size);
    for (size_t i = 0; i < data_size; ++i) {
        result_single[i] = heavy_computation(data[i]);
    }
    auto end_single = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration_single = end_single - start_single;
    std::cout << "Single-threaded execution time: " << std::fixed << std::setprecision(2) << duration_single.count() << " ms" << std::endl;

    // --- Multi-threaded execution ---
    std::cout << "Running multi-threaded computation..." << std::endl;
    ThreadPool pool;
    auto start_multi = std::chrono::high_resolution_clock::now();
    std::vector<double> result_multi = data; // Make a copy to modify
    pool.ForEach(result_multi.begin(), result_multi.end(), [](double& val){
        val = heavy_computation(val);
    });
    auto end_multi = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration_multi = end_multi - start_multi;
    std::cout << "Multi-threaded execution time: " << duration_multi.count() << " ms" << std::endl;

    // --- Verification and Speedup ---
    if (std::equal(result_single.begin(), result_single.end(), result_multi.begin())) {
        std::cout << "Results are identical." << std::endl;
        double speedup = duration_single.count() / duration_multi.count();
        std::cout << "Speedup: " << std::setprecision(2) << speedup << "x" << std::endl;
    } else {
        std::cout << "Error: Results are not identical!" << std::endl;
    }
}

int main() {
    feature_demonstration();
    performance_comparison();
    return 0;
}