#ifndef REALTIMESYSROVER_SAFE_QUEUE_HPP
#define REALTIMESYSROVER_SAFE_QUEUE_HPP

#include <queue>
#include <mutex>


// Thread-safe queue

class SafeQueue {
  private:
    std::queue<std::string> queue;
    mutable std::mutex m;
    std::condition_variable c;

  public:
    SafeQueue() {}

    void enqueue(std::string t) {
        std::lock_guard<std::mutex> lock(m);
        queue.push(t);
        c.notify_one();
    }

    std::string dequeue() {
        std::unique_lock<std::mutex> lock(m);
        while(queue.empty()) {
            c.wait(lock);
        }
        std::string val = queue.front();
        queue.pop();
        return val;
    }
};


#endif // REALTIMESYSROVER_SAFE_QUEUE_HPP
