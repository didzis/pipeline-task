#pragma once

// Objective:
// For data passing between threads, create "universal (templated) queue" which you can use in all (current two) cases.
// Threads are not allowed to block each other executing its task, except in a moment when pushing / popping data in exchange queue.

#include <queue>
#include <mutex>
#include <exception>
#include <condition_variable>


template <typename T>
class Queue {
public:

    Queue() {}

    void push(const T& item) {
        {
            const std::lock_guard<std::mutex> lock(mtx);
            if (closed) {
                throw std::out_of_range("queue is closed");
            }
            queue.push(item);
        }
        cv.notify_one();
    }

    void push(T&& item) {
        {
            const std::lock_guard<std::mutex> lock(mtx);
            if (closed) {
                throw std::out_of_range("queue is closed");
            }
            queue.push(item);
        }
        cv.notify_one();
    }

    void close() { { std::lock_guard<std::mutex> lock(mtx); closed = true; } cv.notify_all(); }  // no more input is accepted

    operator bool() { std::lock_guard<std::mutex> lock(mtx); if (queue.size() > 0) return true; return !closed; }

    bool empty() const { std::lock_guard<std::mutex> lock(mtx); return queue.empty(); }

    // NOTE: return value is expected to be optimized by compiler
    T pop() {
        std::unique_lock<std::mutex> lock(mtx);
        // wait until a value is available or queue is closed
        cv.wait(lock, [&]{ return queue.size() > 0 || closed; });
        if (closed && queue.size() == 0) {
            throw std::out_of_range("queue is closed");
        }
        T v = std::move(queue.front());
        queue.pop();
        return std::move(v);
    }

private:
    std::mutex mtx;
    std::condition_variable cv;
    std::queue<T> queue;
    bool closed = false;
};
