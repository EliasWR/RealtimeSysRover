#include "autonomous_driving/autonomous_driving.hpp"

AutonomousDriving::AutonomousDriving() = default;

/**
 * @brief Add the latest detection.
 *
 * @param detection The latest detection.
 */
void AutonomousDriving::addLatestDetection(std::optional<Detection> &detection) {
    std::unique_lock<std::mutex> guard(_mutex);
    _latest_detection = detection;
}

/**
 * @brief Set the latest command.
 *
 * @param command The latest command.
 */
void AutonomousDriving::setLatestCommand(std::string &command) {
    std::unique_lock<std::mutex> guard(_mutex);
    _latest_command = command;
}

/**
 * @brief Get the latest command.
 *
 * @return std::optional<std::string> The latest command if available, otherwise nullopt.
 */
std::optional<std::string> AutonomousDriving::getLatestCommand() {
    std::unique_lock<std::mutex> guard(_mutex);
    return _latest_command;
}

/**
 * @brief Limiting the radius of the joystick command to the radius limit.
 *
 * @details
 * This function limits the radius of the joystick command to the radius limit.
 *
 * @param x The x coordinate of the joystick command.
 * @param y The y coordinate of the joystick command.
 * @param radiusLimit The radius limit.
 * @return std::pair<int, int> The x and y coordinates of the joystick command.
 */
std::pair<int, int> AutonomousDriving::limitRadius(int x, int y, int radiusLimit) {
    double distance = std::sqrt(x * x + y * y);
    if (distance > radiusLimit) {
        double scale = radiusLimit / distance;
        x = static_cast<int>(x * scale);
        y = static_cast<int>(y * scale);
    }
    return {x, y};
}

/**
 * @brief Calculating the angle of the joystick
 *
 * @details
 * This function calculates the angle of the joystick
 *
 * @param x The x coordinate of the joystick command.
 * @param y The y coordinate of the joystick command.
 * @return double The angle of the joystick command.
 */

double AutonomousDriving::calculateAngle(int x, int y) {
    double angle = std::atan2(y, x) * 180.0 / std::numbers::pi;
    return angle < 0 ? angle + 360 : angle;
}

/**
 * @brief Checking if the angle is within the angle limits.
 *
 * @param angle
 * @param angleLimitDeg
 * @return
 */
bool AutonomousDriving::isAngleWithinLimits(double angle, int angleLimitDeg) {
    const int referenceAngles[] = {90, 270};
    for (int referenceAngle: referenceAngles) {
        double lowerBoundDeg = referenceAngle - angleLimitDeg;
        double upperBoundDeg = referenceAngle + angleLimitDeg;

        if (angle >= lowerBoundDeg && angle <= upperBoundDeg) {
            return true;
        }
    }
    return false;
}

/**
 * @brief Adjusting the angle of the joystick command to the angle limits.
 *
 * @details
 * This function prevents the rover from turning too hard.
 *
 * @param angle
 * @param angleLimitDeg
 * @return
 */
double AutonomousDriving::adjustAngleToBoundary(double angle, int angleLimitDeg) {
    std::pair<int, int> reference_angles = {90, 270};
    double new_angle = angle;

    if (angle < 0) {
        angle += 360;
    }

    double closest_difference = 1000.0;

    for (int reference_angle: {reference_angles.first, reference_angles.second}) {
        double lower_boundary = reference_angle - angleLimitDeg;
        double upper_boundary = reference_angle + angleLimitDeg;

        if (angle >= lower_boundary && angle <= upper_boundary) {
            return angle;
        }

        double lower_difference = std::abs(angle - lower_boundary);
        double upper_difference = std::abs(angle - upper_boundary);

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

/**
 * @brief Converting the polar coordinates of the joystick command to cartesian coordinates.
 *
 * @details
 * This function converts the polar coordinates of the joystick command to cartesian coordinates.
 *
 * @param radius The radius of the joystick command.
 * @param angle The angle of the joystick command.
 * @return std::pair<int, int> The x and y coordinates of the joystick command.
 */
std::pair<int, int> AutonomousDriving::convertToCartesian(int radius, double angle) {
    double radAngle = angle * std::numbers::pi / 180.0;
    int x = static_cast<int>(radius * std::cos(radAngle));
    int y = static_cast<int>(radius * std::sin(radAngle));
    return {x, y};
}

/**
 * @brief Checking if the radius and angle of the joystick command are within the limits.
 *
 * @details
 * This function checks if the radius and angle of the joystick command are within the limits.
 * If the radius or angle are not within the limits, the radius and angle are adjusted to the limits.
 *
 * @param x The x coordinate of the joystick command.
 * @param y The y coordinate of the joystick command.
 * @param radiusLimit The radius limit.
 * @param angleLimitDeg The angle limit.
 * @return std::pair<int, int> The x and y coordinates of the joystick command.
 */
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

/**
 * @brief Formatting the joystick command to the correct format.
 *
 * @details
 * Shape of command: std::string joystick_rover_x_y
 *
 * @param x The x coordinate of the joystick command.
 * @param y The y coordinate of the joystick command.
 * @return std::string The formatted joystick command.
 */
std::string AutonomousDriving::formatCommand(int &x, int &y) const {
    std::string command;
    checkLimits(x, y, _speedLim, _angleLim);
    command = "joystick_rover_" + std::to_string(x) + "_" + std::to_string(y);
    return command;
}

/**
 * @brief Calculating the center of box of the detected object.
 *
 * @details
 * Used for determining if the object is centered in the frame.
 *
 * @param box The bounding box.
 * @return std::pair<int, int> The center of the bounding box with a x and y coordinate.
 */
std::pair<int, int> AutonomousDriving::calculateBoxCenter(const cv::Rect &box) {
    int centerX = box.x + box.width / 2;
    int centerY = box.y + box.height / 2;
    return {centerX, centerY};
}

/**
 * @brief Check if object is centered in the frame.
 *
 * @param boxCenterX
 * @param frameSize
 * @param centerThreshold
 * @return bool True if the object is centered, false otherwise.
 */
bool AutonomousDriving::isObjectCentered(int boxCenterX, const std::pair<int, int> &frameSize, double centerThreshold) {
    return std::abs(boxCenterX - frameSize.first / 2) < centerThreshold * frameSize.first;
}

/**
 * @brief Check if the size of the bounding box is within range.
 *
 * @details
 * Used for determining if the rover needs to back up or drive forward.
 *
 * @param boxWidth The width of the bounding box.
 * @param frameSize The size of the frame.
 * @param minThreshold The minimum threshold for the size of the bounding box.
 * @param maxThreshold The maximum threshold for the size of the bounding box.
 * @return bool True if the size of the bounding box is within range, false otherwise.
 */
bool AutonomousDriving::isSizeInRange(int boxWidth, const std::pair<int, int> &frameSize, double minThreshold,
                                      double maxThreshold) {
    return boxWidth >= frameSize.first * minThreshold && boxWidth <= frameSize.first * maxThreshold;
}

/**
 * @brief Calculating the joystick position
 *
 * @details
 * Calculating the joystick position based on center of the bounding box.
 *
 * @param boxCenterX The x coordinate of the center of the bounding box.
 * @param boxCenterY The y coordinate of the center of the bounding box.
 * @param frameSize The size of the frame.
 * @return std::pair<int, int> The x and y coordinates of the joystick command.
 */
std::pair<int, int>
AutonomousDriving::calculateJoystickPosition(int boxCenterX, int boxCenterY, const std::pair<int, int> &frameSize) {
    int displacementX = frameSize.first / 2 - boxCenterX;
    int displacementY = frameSize.second / 2 - boxCenterY;
    int joystickX = -static_cast<int>((static_cast<double>(displacementX) / frameSize.first) * 100);
    int joystickY = static_cast<int>((static_cast<double>(displacementY) / frameSize.second) * 100);
    return {joystickX, joystickY};
}


/**
 * @brief Determining joystick command based result of other functions.
 *
 * @param isCentered
 * @param isSizeInRange
 * @param joystick
 * @return std::pair<int, int> The x and y coordinates of the joystick command.
 */
std::pair<int, int>
AutonomousDriving::determineJoystickAction(bool isCentered, bool isSizeInRange, const std::pair<int, int> &joystick) {
    if (isCentered && isSizeInRange) {
        return {0, 0};
    } else if (isCentered) {
        return {0, joystick.second};
    } else if (isSizeInRange) {
        return {joystick.first, 0};
    }
    return joystick;
}

/**
 * @brief Interpreting the latest detection.
 *
 * @details
 * Connecting other functions to determine the joystick command.
 *
 * @param detection The latest detection.
 * @return std::pair<int, int> The x and y coordinates of the joystick command.
 */
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

/**
 * @brief Running autonomous command thread.
 *
 * @details
 * Running command that starts separate thread for interpreting the latest detection.
 */
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

/**
 * @brief Stopping autonomous command thread.
 *
 * @details
 * Joining the thread
 */
void AutonomousDriving::stop() {
    _running = false;
    _t.join();
}