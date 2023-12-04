#ifndef REALTIMESYSROVER_GUI_HELPER_HPP
#define REALTIMESYSROVER_GUI_HELPER_HPP

#include <numbers>
#include <iostream>
#include <sstream>
#include <cmath>
#include "nlohmann/json.hpp"

namespace gui_helper {

    constexpr int MAX_SPEED = 255;
    constexpr int MAX_HEADING = 360;
    constexpr int MIN_HEADING = 0;
    constexpr double PI = std::numbers::pi_v<double>;
    constexpr int MAX_JOY_VAL = 200;

    using json = nlohmann::json;

    template<typename T>
    int map(T val, T in_min, T in_max, T out_min, T out_max) {
        return (val - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    }

    struct sRawMotors {
        int left_direction;
        int left_velocity;
        int right_direction;
        int right_velocity;
    };

    struct sMotors {
        int heading;
        int speed;
    };

    std::vector<std::string> splitString(const std::string &str, char delimiter) {
        std::vector<std::string> tokens;
        std::stringstream ss(str);
        std::string token;
        while (std::getline(ss, token, delimiter)) {
            tokens.push_back(token);
        }
        return tokens;
    }

    sMotors joystick_to_heading_and_speed(int joy_x, int joy_y) {
        double magnitude = std::hypot(joy_x, joy_y);
        int heading = static_cast<int>(std::atan2(joy_y, joy_x) * 180 / PI) - 90;
        heading = heading < 0 ? heading + MAX_HEADING : heading;
        heading = map(heading, MIN_HEADING, MAX_HEADING, MAX_HEADING, MIN_HEADING) % MAX_HEADING;
        int speed = static_cast<int>(magnitude * MAX_SPEED / 100);
        return {heading, speed};
    }

    int direction_from_velocity(int value) {
        return (value > 0) ? 1 : (value < 0) ? 2 : 0;
    }

    sRawMotors joystick_to_raw_motors(int joy_x, int joy_y) {
        joy_x = joy_y < 0 ? -joy_x : joy_x;
        int left_velocity = (joy_y + joy_x) * MAX_SPEED / MAX_JOY_VAL;
        int right_velocity = (joy_y - joy_x) * MAX_SPEED / MAX_JOY_VAL;
        return {direction_from_velocity(left_velocity), std::abs(left_velocity),
                direction_from_velocity(right_velocity), std::abs(right_velocity)};
    }

    json create_rover_message(const std::vector<std::string>& tokens) {
        json rover_message = {{"command", ""}, {"drive_mode", "tank"}, {"left_direction", ""}, {"left_velocity", ""}, {"right_direction", ""}, {"right_velocity", ""}, {"heading", ""}, {"speed", ""}};
        //

        // Rover Messages based on tokens

        //

        return rover_message;
    }

    std::string message_handler(const std::string &message) {
        std::vector<std::string> tokens = splitString(message, '_');
        json rover_message = create_rover_message(tokens);
        return rover_message.dump();
    }

}

#endif // REALTIMESYSROVER_GUI_HELPER_HPP
