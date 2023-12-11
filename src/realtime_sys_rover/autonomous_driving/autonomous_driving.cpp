#include "autonomous_driving/autonomous_driving.hpp"

AutonomousDriving::AutonomousDriving() = default;

void AutonomousDriving::addLatestDetection(std::optional<Detection> &detection) {
    std::unique_lock<std::mutex> guard(_mutex);
    _latest_detection = detection;
}

void AutonomousDriving::setLatestCommand(std::string &command) {
    std::unique_lock<std::mutex> guard(_mutex);
    _latest_command = command;
}

std::optional<std::string> AutonomousDriving::getLatestCommand() {
    std::unique_lock<std::mutex> guard(_mutex);
    return _latest_command;
}

std::pair<int, int> AutonomousDriving::limitRadius(int x, int y, int radiusLimit) {
    double distance = std::sqrt(x * x + y * y);
    if (distance > radiusLimit) {
        double scale = radiusLimit / distance;
        x = static_cast<int>(x * scale);
        y = static_cast<int>(y * scale);
    }
    return {x, y};
}

double AutonomousDriving::calculateAngle(int x, int y) {
    double angle = std::atan2(y, x) * 180.0 / std::numbers::pi;
    return angle < 0 ? angle + 360 : angle;
}

bool AutonomousDriving::isAngleWithinLimits(double angle, int angleLimitDeg) {
    const int referenceAngles[] = {90, 270};
    for (int referenceAngle : referenceAngles) {
        double lowerBoundDeg = referenceAngle - angleLimitDeg;
        double upperBoundDeg = referenceAngle + angleLimitDeg;

        if (angle >= lowerBoundDeg && angle <= upperBoundDeg) {
            return true;
        }
    }
    return false;
}
/*
double AutonomousDriving::adjustAngleToBoundary(double angle, int angleLimitDeg) {
    const int referenceAngles[] = {90, 270};
    double closestDifference = std::numeric_limits<double>::max();
    double adjustedAngle = angle;

    angle = fmod(angle, 360);
    if (angle < 0) angle += 360;

    for (int referenceAngle : referenceAngles) {
        double lowerBoundDeg = referenceAngle - angleLimitDeg;
        double upperBoundDeg = referenceAngle + angleLimitDeg;

        if (angle >= lowerBoundDeg && angle <= upperBoundDeg) {
            return angle;
        }

        double lowerDiff = std::abs(angle - lowerBoundDeg);
        double upperDiff = std::abs(angle - upperBoundDeg);

        if (lowerDiff < closestDifference) {
            closestDifference = lowerDiff;
            adjustedAngle = (lowerDiff <= angleLimitDeg) ? angle : lowerBoundDeg;
        }
        if (upperDiff < closestDifference) {
            closestDifference = upperDiff;
            adjustedAngle = (upperDiff <= angleLimitDeg) ? angle : upperBoundDeg;
        }
    }

    return adjustedAngle;
}
*/
double AutonomousDriving::adjustAngleToBoundary(double angle, int angleLimitDeg) {
    std::pair<int, int> reference_angles = {90, 270};
    double new_angle = angle;

    if (angle < 0) {
        angle += 360;
    }

    double closest_difference = 1000.0;

    for (int reference_angle : {reference_angles.first, reference_angles.second}) {
        double lower_boundary = reference_angle - angleLimitDeg;
        double upper_boundary = reference_angle + angleLimitDeg;

        if (angle >= lower_boundary && angle <= upper_boundary) {
            std::cout << lower_boundary << upper_boundary << angle << std::endl;
            return angle;
        }

        double lower_difference = std::abs(angle - lower_boundary);
        double upper_difference = std::abs(angle - upper_boundary);

        std::cout << lower_difference << upper_difference << angle << std::endl;

        if (lower_difference < closest_difference) {
            closest_difference = lower_difference;
            new_angle = lower_boundary;
        }
        if (upper_difference < closest_difference) {
            closest_difference = upper_difference;
            new_angle = upper_boundary;
        }
    }
    return new_angle;
}

std::pair<int, int> AutonomousDriving::convertToCartesian(int radius, double angle) {
    double radAngle = angle * std::numbers::pi / 180.0;
    int x = static_cast<int>(radius * std::cos(radAngle));
    int y = static_cast<int>(radius * std::sin(radAngle));
    return {x, y};
}

std::pair<int, int> AutonomousDriving::checkLimits(int &x, int &y, const int &radiusLimit, const int &angleLimitDeg) {
    bool xIsZero = (x == 0);
    bool yIsZero = (y == 0);

    std::tie(x, y) = limitRadius(x, y, radiusLimit);
    double angle = calculateAngle(x, y);

    if (!isAngleWithinLimits(angle, angleLimitDeg)) {
        angle = adjustAngleToBoundary(angle, angleLimitDeg);
    }

    if (!xIsZero || !yIsZero) {
        std::tie(x, y) = convertToCartesian(radiusLimit, angle);
    }

    return std::make_pair(x, y);
}

// Shape of command: std::string joystick_rover_x_y
std::string AutonomousDriving::formatCommand(int &x, int &y) const {
    std::string command;
    checkLimits(x, y, _speedLim, _angleLim);
    command = "joystick_rover_" + std::to_string(x) + "_" + std::to_string(y);
    return command;
}

std::pair<int, int> AutonomousDriving::calculateBoxCenter(const cv::Rect &box) {
    int centerX = box.x + box.width / 2;
    int centerY = box.y + box.height / 2;
    return {centerX, centerY};
}

bool AutonomousDriving::isObjectCentered(int boxCenterX, const std::pair<int, int> &frameSize, double centerThreshold) {
    return std::abs(boxCenterX - frameSize.first / 2) < centerThreshold * frameSize.first;
}

bool AutonomousDriving::isSizeInRange(int boxWidth, const std::pair<int, int> &frameSize, double minThreshold, double maxThreshold) {
    return boxWidth >= frameSize.first * minThreshold && boxWidth <= frameSize.first * maxThreshold;
}

std::pair<int, int> AutonomousDriving::calculateJoystickPosition(int boxCenterX, int boxCenterY, const std::pair<int, int> &frameSize) {
    int displacementX = frameSize.first / 2 - boxCenterX;
    int displacementY = frameSize.second / 2 - boxCenterY;
    int joystickX = -static_cast<int>((static_cast<double>(displacementX) / frameSize.first) * 100);
    int joystickY = static_cast<int>((static_cast<double>(displacementY) / frameSize.second) * 100);
    return {joystickX, joystickY};
}

std::pair<int, int> AutonomousDriving::determineJoystickAction(bool isCentered, bool isSizeInRange, const std::pair<int, int> &joystick) {
    if (isCentered && isSizeInRange) {
        return {0, 0};
    } else if (isCentered) {
        return {0, joystick.second};
    } else if (isSizeInRange) {
        return {joystick.first, 0};
    }
    return joystick;
}

std::pair<int, int> AutonomousDriving::interpretLatestDetection(Detection &detection) {
    if (detection.boxes.empty()) {
        return {0, 0};
    }

    const double centerThreshold = 0.1;
    const double sizeMinThreshold = 0.1;
    const double sizeMaxThreshold = 0.4;

    for (int idx = 0; idx < detection.classIds.size(); idx++) {
        auto label = detection.classIds[idx];
        if (label == 39) { // bottle
            auto box = detection.boxes[idx];
            auto [boxCenterX, boxCenterY] = calculateBoxCenter(box);

            bool isCentered = isObjectCentered(boxCenterX, detection.frameSize, centerThreshold);
            bool sizeInRange = isSizeInRange(box.width, detection.frameSize, sizeMinThreshold, sizeMaxThreshold);
            auto joystick = calculateJoystickPosition(boxCenterX, boxCenterY, detection.frameSize);

            return determineJoystickAction(isCentered, sizeInRange, joystick);
        }
    }

    return {0, 0};
}

void AutonomousDriving::run() {
    _running = true;
    _t = std::thread([&]() {
        while (_running) {
            if (_latest_detection.has_value()) {
                std::pair<int, int> joystick = interpretLatestDetection(_latest_detection.value());
                std::string command = formatCommand(joystick.first, joystick.second);
                setLatestCommand(command);
            } else {
                std::string command = "joystick_rover_0_0";
                setLatestCommand(command);
            }
        }
    });
}

void AutonomousDriving::stop() {
    _running = false;
    _t.join();
}