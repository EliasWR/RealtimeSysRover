#ifndef REALTIMESYSROVER_AUTONOMOUS_DRIVING_HPP
#define REALTIMESYSROVER_AUTONOMOUS_DRIVING_HPP

#include "helpers/detection_helper.hpp"
#include <mutex>
#include <numbers>
#include <thread>
#include <optional>

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
    static std::pair<int, int> limitRadius(int x, int y, int radiusLimit);

    static double calculateAngle(int x, int y);

    static bool isAngleWithinLimits(double angle, int angleLimitDeg);

    static double adjustAngleToBoundary(double angle, int angleLimitDeg);

    static std::pair<int, int> convertToCartesian(int radius, double angle);

    static std::pair<int, int> calculateBoxCenter(const cv::Rect &box);

    static bool isObjectCentered(int boxCenterX, const std::pair<int, int> &frameSize, double centerThreshold);

    static bool
    isSizeInRange(int boxWidth, const std::pair<int, int> &frameSize, double minThreshold, double maxThreshold);

    static std::pair<int, int>
    calculateJoystickPosition(int boxCenterX, int boxCenterY, const std::pair<int, int> &frameSize);

    static std::pair<int, int>
    determineJoystickAction(bool isCentered, bool isSizeInRange, const std::pair<int, int> &joystick);

    std::thread _t;
    std::optional<std::string> _latest_command;
    std::mutex _mutex;
    std::optional<Detection> _latest_detection;
    int _speedLim{60};// -100 < x < 100, -100 < y < 100
    int _angleLim{20};// 30 deg to either side of reference angle
};


#endif//REALTIMESYSROVER_AUTONOMOUS_DRIVING_HPP