# C++ Thread Pool

A modern and feature-rich C++17 thread pool implementation designed for performance and ease of use.

## Description

This project provides a versatile and efficient thread pool that can be easily integrated into any C++ project. It supports various parallel programming patterns, including task enqueuing, parallel for-each, and transform-reduce, making it a powerful tool for accelerating computationally intensive applications.

## Features

- **Simple and Modern C++17 Interface:** Easy to integrate and use.
- **Task Enqueuing:** Enqueue any callable object (functions, lambdas, functors) and get a `std::future` back.
- **Parallel For-Each:** Apply a function to a range of elements in parallel.
- **Parallel Transform-Reduce:** Parallelize the map-reduce pattern.
- **Automatic Thread Management:** The pool manages the lifecycle of threads.
- **Header-Only Option:** Can be used as a header-only library for easy integration.
- **Extensively Tested:** Comes with a comprehensive suite of unit tests using Google Test.

## Getting Started

### Prerequisites

- C++17 compatible compiler (e.g., GCC, Clang, MSVC)
- CMake 3.10 or higher

### Building

1. **Clone the repository:**
   ```bash
   git clone https://github.com/geb0598/cpp-thread-pool.git
   cd cpp-thread-pool
   ```

2. **Create a build directory:**
   ```bash
   mkdir build
   cd build
   ```

3. **Configure and build the project:**
   ```bash
   cmake ..
   cmake --build .
   ```

## Usage

### Basic Task Enqueuing

```cpp
#include "thread_pool.h"
#include <iostream>

int main() {
    ThreadPool pool;

    auto future = pool.Enqueue([]() {
        return 42;
    });

    std::cout << "The answer is: " << future.get() << std::endl;

    return 0;
}
```

### Parallel For-Each

```cpp
#include "thread_pool.h"
#include <iostream>
#include <vector>

int main() {
    ThreadPool pool;
    std::vector<int> numbers = {1, 2, 3, 4, 5};

    pool.ForEach(numbers.begin(), numbers.end(), [](int& n) {
        n *= 2;
    });

    for (int n : numbers) {
        std::cout << n << " ";
    }
    // Output: 2 4 6 8 10

    return 0;
}
```

### Parallel Transform-Reduce

```cpp
#include "thread_pool.h"
#include <iostream>
#include <vector>
#include <numeric>

int main() {
    ThreadPool pool;
    std::vector<int> numbers = {1, 2, 3, 4, 5};

    int sum_of_squares = pool.TransformReduce(numbers.begin(), numbers.end(), 0,
        [](int n) { return n * n; },
        std::plus<int>()
    );

    std::cout << "Sum of squares: " << sum_of_squares << std::endl;
    // Output: Sum of squares: 55

    return 0;
}
```

## Building and Testing

All build commands should be run from the `build` directory. If you haven't already, create it and move into it:

```bash
mkdir build
cd build
```

From within the `build` directory, you can build the project and run the tests:

1. **Build the project:**
   ```bash
   cmake --build .
   ```

2. **Run the tests:**
   ```bash
   ctest
   ```

## Contributing

Contributions are welcome! Please feel free to submit a pull request.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgements

This project is heavily inspired by the thread pool implementation from [progschj/ThreadPool](https://github.com/progschj/ThreadPool).