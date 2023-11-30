#include "autonomous_driving/autonomous_driving.hpp"

AutonomousDriving::AutonomousDriving() {}

void ObjectDetection::addLatestDetection(Detection& detection) {
    std::unique_lock<std::mutex> guard(mutex);
    _latest_frame = frame;
}

void AutonomousDriving::run() {
  _running = true;
  _t = std::thread([&] () {
    while (_running) {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      std::string command = "stop";
      if (_latest_detection.has_value()) {
        command = "drive";
      }
      mutex.lock();
      _latest_command = command;
      mutex.unlock();
    }
  });
}