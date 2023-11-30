#include "autonomous_driving/autonomous_driving.hpp"

AutonomousDriving::AutonomousDriving() = default;

void AutonomousDriving::addLatestDetection(Detection& detection) {
    std::unique_lock<std::mutex> guard(_mutex);
    _latest_detection = detection;
}

// For future check if they are within 30 degrees of forward or backwards
std::pair<int, int> AutonomousDriving::checkLimits (int& x, int& y, const int& radiusLimit, const int& angleLimitDeg) {
    const int referenceAngle1 = 90;
    const int referenceAngle2 = 270;
    // Keep the x and y within the radius limit
    double distance = std::sqrt(x * x + y * y);
    if (distance > radiusLimit) {
        double scale = radiusLimit / distance;
        x = static_cast<int>(x * scale);
        y = static_cast<int>(y * scale);
    }

    // Calculate angle in degrees
    double angle = std::atan2(y, x) * 180.0 / M_PI;
    if (angle < 0) {
        angle += 360;
    }

    // Check if angle is within the degrees limit for either reference angle
    bool withinLimits = false;
    for (int referenceAngle : {referenceAngle1, referenceAngle2}) {
        double lowerBoundDeg = referenceAngle - angleLimitDeg;
        double upperBoundDeg = referenceAngle + angleLimitDeg;

        if (angle >= lowerBoundDeg && angle <= upperBoundDeg) {
            withinLimits = true;
            break;
        }
    }

    // If not within limits, adjust to the nearest boundary
    if (!withinLimits) {
        double closestDifference = std::numeric_limits<double>::max();
        double adjustedAngle = 0;

        for (int referenceAngle : {referenceAngle1, referenceAngle2}) {
            double lowerBoundDeg = referenceAngle - angleLimitDeg;
            double upperBoundDeg = referenceAngle + angleLimitDeg;

            double lowerDiff = std::abs(angle - lowerBoundDeg);
            double upperDiff = std::abs(angle - upperBoundDeg);

            if (lowerDiff < closestDifference) {
                closestDifference = lowerDiff;
                adjustedAngle = lowerBoundDeg;
            }
            if (upperDiff < closestDifference) {
                closestDifference = upperDiff;
                adjustedAngle = upperBoundDeg;
            }
        }

        angle = adjustedAngle;
    }

    // Convert back to Cartesian coordinates
    double radAngle = angle * M_PI / 180.0;
    x = static_cast<int>(radiusLimit * std::cos(radAngle));
    y = static_cast<int>(radiusLimit * std::sin(radAngle));

    return std::make_pair(x, y);
}

std::string AutonomousDriving::formatCommand(int x, int y) {
    // Shape of command: std::string joystick_rover_x_y
    std::string command;
    checkLimits(x, y, _speedLim, _angleLim);
    command = "joystick_rover_" + std::to_string(x) + "_" + std::to_string(y);
    return command;
}

void AutonomousDriving::run() {
  _running = true;
  _t = std::thread([&] () {
    while (_running) {

      std::string command = "stop";

      if (_latest_detection.has_value()) {
        command = "drive";
      }

      _mutex.lock();
      _latest_command = command;
      _mutex.unlock();
    }
  });
}

/*
Command AutonomousDriving::interpretLatestDetection() {
    if (!_latest_detection.has_value()) {
        return Command::stop;
    }
    auto detection = _latest_detection.value();
    auto classIds = detection.classIds;
    auto confidences = detection.confidences;
    auto boxes = detection.boxes;
    for (int idx = 0; idx < classIds.size(); idx++) {
        if (classIds[idx] < _classNames.size()) {
            auto label = _classNames[classIds[idx]];
            if (label == "person") {
                if (confidences[idx] > 0.5) {
                    auto box = boxes[idx];
                    auto width = box.width;
                    auto height = box.height;
                    auto x = box.x;
                    auto y = box.y;
                    if (width > height) {
                        if (x < 320) {
                            return Command::stop;
                        } else {
                            return Command::drive;
                        }
                    } else {
                        if (y < 240) {
                            return Command::stop;
                        } else {
                            return Command::drive;
                        }
                    }
                }
            }
        } else {
            std::cerr << "Class ID " << classIds[idx] << " is out of range." << std::endl;
        }
    }
    return Command::stop;

}
*/