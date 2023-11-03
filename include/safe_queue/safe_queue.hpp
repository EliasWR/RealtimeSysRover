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

public:
  SafeQueue() {
  }

  void enqueue(T t) {
    std::lock_guard<std::mutex> lock(m);
    queue.push(t);
    cv.notify_one();
  }

  std::string dequeue() {
    std::unique_lock<std::mutex> lock(m);
    while (queue.empty()) {
      cv.wait(lock);
    }
    T val = queue.front();
    queue.pop();
    return val;
  }
};


#endif// REALTIMESYSROVER_SAFE_QUEUE_HPP
