#include "autonomous_driving/autonomous_driving.hpp"

AutonomousDriving::AutonomousDriving() = default;

void AutonomousDriving::addLatestDetection (std::optional<Detection>& detection) {
    std::unique_lock<std::mutex> guard(_mutex);
    _latest_detection = detection;
}

void AutonomousDriving::setLatestCommand(std::string& command) {
    std::unique_lock<std::mutex> guard(_mutex);
    _latest_command = command;
}

std::optional <std::string> AutonomousDriving::getLatestCommand() {
    std::unique_lock<std::mutex> guard(_mutex);
    return _latest_command;
}

// For future check if they are within 30 degrees of forward or backwards
std::pair<int, int> AutonomousDriving::checkLimits (int& x, int& y, const int& radiusLimit, const int& angleLimitDeg) {
    const int referenceAngle1 = 90;
    const int referenceAngle2 = 270;

    bool xIsZero = (x == 0);
    bool yIsZero = (y == 0);

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
    if (!xIsZero) {
        x = static_cast<int>(radiusLimit * std::cos(radAngle));
    }
    if (!yIsZero) {
        y = static_cast<int>(radiusLimit * std::sin(radAngle));
    }

    return std::make_pair(x, y);
}

// Shape of command: std::string joystick_rover_x_y
std::string AutonomousDriving::formatCommand(int& x, int& y) const {
    std::string command;
    checkLimits(x, y, _speedLim, _angleLim);
    command = "joystick_rover_" + std::to_string(x) + "_" + std::to_string(y);
    return command;
}



std::pair<int, int> AutonomousDriving::interpretLatestDetection (Detection& detection) {
    std::pair<int, int> joystick;
    if (detection.boxes.empty()){
        joystick = std::make_pair(0, 0);
        return joystick;
    }

    // Define thresholds for centering and size
    const double centerThreshold = 0.1; // 0.1
    const double sizeMinThreshold = 0.1; // 0.1
    const double sizeMaxThreshold = 0.3; // 0.3

    for (int idx = 0; idx < detection.classIds.size(); idx++) {
        auto label = detection.classIds[idx];
        //if (label == 0) { // person
        if (label == 39) {  // bottle
            auto box = detection.boxes[idx];

            // Calculating center of object
            int boxCenterX = box.x + box.width / 2;
            int boxCenterY = box.y + box.height / 2;

            // Checking if object is centered and within size range
            bool isCentered =   std::abs(boxCenterX) < detection.frameSize.first/2 + centerThreshold * detection.frameSize.first &&
                                std::abs(boxCenterX) > detection.frameSize.first/2 - centerThreshold * detection.frameSize.first;


            // std::cout << "boxCenterX: " << boxCenterX << "Threshold: " << detection.frameSize.first/2 - centerThreshold << " , " << detection.frameSize.first/2 + centerThreshold << std::endl;
            // std::cout << "Is centered: " << isCentered << std::endl;
            bool isSizeInRange = box.width >= detection.frameSize.first * sizeMinThreshold &&
                                 box.width <= detection.frameSize.first * sizeMaxThreshold;//  &&
                                 // box.height >= detection.frameSize.second * sizeMinThreshold &&
                                 // box.height <= detection.frameSize.second * sizeMaxThreshold;

            // Controlling joystick
            int displacementX = detection.frameSize.first/2 - boxCenterX;
            int displacementY = detection.frameSize.first/2 - boxCenterY;

            // Calculating joystick position
            joystick.first = -static_cast<int>((static_cast<double>(displacementX) / detection.frameSize.first) * 100);
            joystick.second = static_cast<int>((static_cast<double>(displacementY) / detection.frameSize.second) * 100);

            if (isCentered && isSizeInRange) {
                return std::make_pair(0, 0);
            } else if (isCentered) {
                return std::make_pair(0, joystick.second);
            } else if (isSizeInRange) {
                return std::make_pair(joystick.first, 0);
            }
            break;
        }
    }
    return joystick;
}

void AutonomousDriving::run() {
  _running = true;
  _t = std::thread([&] () {
    while (_running) {
        if (_latest_detection.has_value()) {
            std::pair<int, int> joystick = interpretLatestDetection(_latest_detection.value());
            std::string command = formatCommand(joystick.first, joystick.second);
            setLatestCommand(command);
        }
    }
  });
}

void AutonomousDriving::stop() {
  _running = false;
  _t.join();
}