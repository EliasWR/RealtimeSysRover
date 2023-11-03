#ifndef REALTIMESYSROVER_SAFE_QUEUE_HPP
#define REALTIMESYSROVER_SAFE_QUEUE_HPP

#include <mutex>
#include <queue>

// Thread-safe queue

template<typename T>
class SafeQueue {
private:
  std::queue<T> queue;
  mutable std::mutex m;
  std::condition_variable cv;
  bool stop_flag = false;  // Added stop flag

public:
  SafeQueue() = default;

  void enqueue(T t) {
    std::lock_guard<std::mutex> lock(m);
    queue.push(t);
    cv.notify_one();
  }

  std::optional<T> dequeue() {
    std::unique_lock<std::mutex> lock(m);
    while (queue.empty() && !stop_flag) {  // Also check stop_flag
      cv.wait(lock);
    }
    if (stop_flag && queue.empty()) {  // If stopped and queue is empty, return nothing
      return std::nullopt;
    }
    T val = queue.front();
    queue.pop();
    return val;
  }

  void stop() {  // Method to stop the queue
    {
      std::lock_guard<std::mutex> lock(m);
      stop_flag = true;
    }
    cv.notify_all();  // Wake up any waiting threads
  }
};


#endif// REALTIMESYSROVER_SAFE_QUEUE_HPP
