#include "thread_pool.h"

ThreadPool::ThreadPool(
    std::function<void()> on_thread_start,
    std::function<void()> on_thread_stop
) : ThreadPool(std::thread::hardware_concurrency(), on_thread_start, on_thread_stop) {}

ThreadPool::ThreadPool(
    size_t num_threads,
    std::function<void()> on_thread_start,
    std::function<void()> on_thread_stop
) : num_threads_(num_threads), is_terminated_(false) {
    if (num_threads == 0) {
        throw std::runtime_error("The number of threads must be larger than 0.");
    }

    threads_.reserve(num_threads);

    for (size_t i = 0; i < num_threads; ++i) {
        threads_.emplace_back([this, on_thread_start, on_thread_stop] {
            if (on_thread_start) {
                on_thread_start();
            }

            while (true) {
                std::unique_lock<std::mutex> lock(this->m_task_);
                this->cv_task_.wait(lock, [this]() { return !this->tasks_.empty() || is_terminated_; });
                if (this->tasks_.empty() && is_terminated_) {
                    return;
                }
                std::function<void()> task = tasks_.front();
                tasks_.pop();
                lock.unlock();

                task();
            }

            if (on_thread_stop) {
                on_thread_stop();
            }
        }); 
    }
}

ThreadPool::~ThreadPool() {
    {
        std::lock_guard<std::mutex> lock(m_task_); 
        is_terminated_ = true;
    }

    cv_task_.notify_all();

    for (auto& thread : threads_) {
        thread.join();
    }
}
