#ifndef REALTIMESYSROVER_SAFE_QUEUE_HPP
#define REALTIMESYSROVER_SAFE_QUEUE_HPP

#include <mutex>
#include <queue>

// Thread-safe queue

template<typename T>
class SafeQueue {
private:
  std::queue<T> _queue;
  mutable std::mutex _m;

public:
  SafeQueue() = default;

  void enqueue(T t) {
    std::unique_lock<std::mutex> lock(_m);
    _queue.push(t);
  }

  void emplace(T t) {
    std::unique_lock<std::mutex> lock(_m);
    _queue.emplace(t);
  }

  std::optional<T> dequeue() {
    std::unique_lock<std::mutex> lock{_m};
    if (_queue.empty()) {
      return std::nullopt;
    }
    T val = _queue.front();
    _queue.pop();
    return val;
  }
};


#endif// REALTIMESYSROVER_SAFE_QUEUE_HPP
