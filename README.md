# C++ Thread Pool

A simple, robust, and feature-rich thread pool implementation in C++17.

## Features

-   **Simple API:** Easily enqueue tasks and get results via `std::future`.
-   **Parallel Algorithms:** Includes `ForEach` and `TransformReduce` for parallel operations on ranges.
-   **Automatic Thread Management:** Manages a pool of worker threads to execute tasks concurrently.
-   **Cross-Platform:** Built with CMake for cross-platform compatibility.

## Requirements

-   CMake (version 3.10 or later)
-   A C++17 compatible compiler (e.g., GCC, Clang, MSVC)

## How to Build

1.  **Clone the repository:**
    ```bash
    git clone <repository-url>
    cd cpp-thread-pool
    ```

2.  **Create a build directory:**
    ```bash
    mkdir build
    cd build
    ```

3.  **Configure with CMake:**
    ```bash
    cmake ..
    ```
    *On Windows, you may need to specify a generator, e.g., `cmake .. -G "Visual Studio 17 2022"`.*

4.  **Compile the project:**
    ```bash
    cmake --build .
    ```
    *Alternatively, you can use the native build system (e.g., `make` on Linux/macOS or open the `.sln` file in Visual Studio on Windows).*

## Running the Demo

The demo program showcases the thread pool's features and includes a performance benchmark.

After building the project, you can run the demo from the `build` directory:

```bash
# On Linux or macOS
./demo/thread_pool_demo

# On Windows
.\demo\Debug\thread_pool_demo.exe
```

## Running Tests

The project uses GoogleTest for unit testing. To run the tests, you can use CTest from the `build` directory:

```bash
ctest
```
