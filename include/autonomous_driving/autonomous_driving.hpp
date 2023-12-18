#ifndef REALTIMESYSROVER_AUTONOMOUS_DRIVING_HPP
#define REALTIMESYSROVER_AUTONOMOUS_DRIVING_HPP

#include "helpers/detection_helper.hpp"
#include <mutex>
#include <numbers>
#include <optional>
#include <thread>

class AutonomousDriving {
public:
  AutonomousDriving();
  void setLatestCommand(std::string &command);
  std::optional<std::string> getLatestCommand();
  void run();
  void stop();
  static std::pair<int, int> checkLimits(int x, int y, int radius_limit, int angle_limit_deg);
  std::string formatCommand(int x, int y) const;
  void addLatestDetection(std::optional<Detection> &detection);
  static std::pair<int, int> interpretLatestDetection(Detection &detection);
  std::atomic<bool> _running {false};

private:
  static std::pair<int, int> limitRadius(int x, int y, int radius_limit);
  static double calculateAngle(int x, int y);
  static bool isAngleWithinLimits(double angle, int angle_limit_deg);
  static double adjustAngleToBoundary(double angle, int angle_limit_deg);
  static std::pair<int, int> convertToCartesian(int radius, double angle);
  static std::pair<int, int> calculateBoxCenter(const cv::Rect &box);
  static bool isObjectCentered(int box_center_x, const std::pair<int, int> &frame_size, double center_threshold);
  static bool
  isSizeInRange(int box_width, const std::pair<int, int> &frame_size, double min_threshold, double max_threshold);
  static std::pair<int, int>
  calculateJoystickPosition(int box_center_x, int box_center_y, const std::pair<int, int> &frame_size);
  static std::pair<int, int>
  determineJoystickAction(bool is_centered, bool is_size_in_range, const std::pair<int, int> &joystick);

  std::thread _t;
  std::optional<std::string> _latest_command;
  std::mutex _mutex;
  std::optional<Detection> _latest_detection;
  int _speed_lim = 60;// -100 < x < 100, -100 < y < 100
  int _angle_lim = 30;// 30 deg to either side of reference angle

  friend class AutonomousDrivingClassTest;
};


#endif//REALTIMESYSROVER_AUTONOMOUS_DRIVING_HPP