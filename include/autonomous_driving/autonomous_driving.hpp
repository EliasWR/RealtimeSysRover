#ifndef REALTIMESYSROVER_AUTONOMOUS_DRIVING_HPP
#define REALTIMESYSROVER_AUTONOMOUS_DRIVING_HPP

#include "helpers/detection_helper.hpp"
#include <mutex>
#include <numbers>
#include <thread>

class AutonomousDriving {
public:
  AutonomousDriving();
  void setLatestCommand(std::string &command);
  std::optional<std::string> getLatestCommand();
  void run();
  void stop();
  static std::pair<int, int> checkLimits(int &x, int &y, const int &radiusLimit, const int &angleLimitDeg);
  std::string formatCommand(int &x, int &y) const;
  void addLatestDetection(std::optional<Detection> &detection);
  static std::pair<int, int> interpretLatestDetection(Detection &detection);

  bool _running{false};

private:
  std::thread _t;
  std::optional<std::string> _latest_command;
  std::mutex _mutex;
  std::optional<Detection> _latest_detection;
  int _speedLim{60};// -100 < x < 100, -100 < y < 100
  int _angleLim{20};// 30 deg to either side of reference angle
};


#endif//REALTIMESYSROVER_AUTONOMOUS_DRIVING_HPP