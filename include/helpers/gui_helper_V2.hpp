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
    constexpr int MAX_JOY_VAL = 100;

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
        heading = map(heading, MIN_HEADING, MAX_HEADING, MAX_HEADING, MIN_HEADING) % MAX_HEADING; // Flip the heading

        int speed = static_cast<int>(magnitude * MAX_SPEED / 100);

        return {heading, speed};
    }

    int direction_from_velocity(int value) {
        return (value > 0) ? 1 : (value < 0) ? 2 : 0;
    }

    sRawMotors joystick_to_raw_motors(int joy_x, int joy_y) {
        joy_x = joy_y < 0 ? -joy_x : joy_x; // Flip the x axis if the y axis is negative
        int left_velocity = (joy_y + joy_x) * MAX_SPEED / (MAX_JOY_VAL * 2);
        int right_velocity = (joy_y - joy_x) * MAX_SPEED / (MAX_JOY_VAL * 2);
        return {direction_from_velocity(left_velocity), std::abs(left_velocity),
                direction_from_velocity(right_velocity), std::abs(right_velocity)};
    }

    json createRoverMessage(const std::vector<std::string>& tokens) {
        auto command = tokens[0];
        json rover_message = {{"command", ""}};
        if (command == "stop") {
            rover_message["command"] = "stop";
            rover_message["drive_mode"] = "tank";
            rover_message["left_direction"] = "0";
            rover_message["left_velocity"] = "0";
            rover_message["right_direction"] = "0";
            rover_message["right_velocity"] = "0";
            rover_message["speed"] = "0";
        }
        else if (command == "move") {
            rover_message["command"] = "move";
            rover_message["drive_mode"] = "tank";
            if (tokens[1] == "forward") {
              rover_message["left_direction"] = "1";
              rover_message["left_velocity"] = "255";
              rover_message["right_direction"] = "1";
              rover_message["right_velocity"] = "255";
            } else if (tokens[1] == "backward") {
              rover_message["left_direction"] = "2";
              rover_message["left_velocity"] = "255";
              rover_message["right_direction"] = "2";
              rover_message["right_velocity"] = "255";
            } else {
              std::cout << "Invalid move command" << std::endl;
            }
        }
        else if (command == "turn"){
            if (tokens[1] == "left")
            {
              rover_message["command"] = "move";
              rover_message["drive_mode"] = "tank";
              rover_message["left_direction"] = "1";
              rover_message["left_velocity"] = "80";
              rover_message["right_direction"] = "1";
              rover_message["right_velocity"] = "255";
            }
            else if (tokens[1] == "right")
            {
              rover_message["command"] = "move";
              rover_message["drive_mode"] = "tank";
              rover_message["left_direction"] = "1";
              rover_message["left_velocity"] = "255";
              rover_message["right_direction"] = "1";
              rover_message["right_velocity"] = "80";
            }
            else if (tokens[1] == "around")
            {
              if (tokens[2] == "left")
              {
                rover_message["command"] = "move";
                rover_message["drive_mode"] = "tank";
                rover_message["left_direction"] = "2";
                rover_message["left_velocity"] = "255";
                rover_message["right_direction"] = "1";
                rover_message["right_velocity"] = "255";
              }
              if (tokens[2] == "right")
              {
                rover_message["command"] = "move";
                rover_message["drive_mode"] = "tank";
                rover_message["left_direction"] = "1";
                rover_message["left_velocity"] = "255";
                rover_message["right_direction"] = "2";
                rover_message["right_velocity"] = "255";
              }
            }
            else
            {
              std::cout << "Invalid turn command" << std::endl;
            }
        }

        else if (command == "joystick") {
            if (tokens[1] == "rover") {
              rover_message["command"] = "move";
              int joy_x = std::stoi(tokens[2]);
              int joy_y = std::stoi(tokens[3]);

              if (joy_x == 0 && joy_y == 0) {
                rover_message["command"] = "stop";
                rover_message["drive_mode"] = "tank";
                rover_message["left_direction"] = "0";
                rover_message["left_velocity"] = "0";
                rover_message["right_direction"] = "0";
                rover_message["right_velocity"] = "0";
                rover_message["speed"] = "0";
              } else {

                auto h_and_s = joystick_to_heading_and_speed(joy_x, joy_y);
                rover_message["drive_mode"] = "heading";
                rover_message["heading"] = h_and_s.heading;
                rover_message["speed"] = h_and_s.speed;

                auto raw_motors = joystick_to_raw_motors(joy_x, joy_y);
                rover_message["drive_mode"] = "tank";
                rover_message["left_direction"] = raw_motors.left_direction;
                rover_message["left_velocity"] = raw_motors.left_velocity;
                rover_message["right_direction"] = raw_motors.right_direction;
                rover_message["right_velocity"] = raw_motors.right_velocity;
              }

            } else if (tokens[1] == "camera") {
              int joy_x = std::stoi(tokens[2]);
              int joy_y = std::stoi(tokens[3]);
              auto h_and_s = joystick_to_heading_and_speed(joy_x, joy_y);
              std::cout << "Joystick: " << joy_x << ", " << joy_y << std::endl;
              std::cout << "Camera: " << h_and_s.heading << ", " << h_and_s.speed << std::endl;
            } else {
              std::cout << "Invalid joystick command" << std::endl;
            }

        } else {
            std::cout << "Invalid command" << std::endl;
            for (auto &token : tokens){
                std::cout << token << " ";
            }
            std::cout << std::endl;
        }

        rover_message["drive_mode"] = "tank";

        return rover_message;
    }

    std::string message_handler(const std::string &message) {
        std::vector<std::string> tokens = splitString(message, '_');
        json rover_message = createRoverMessage(tokens);
        return rover_message.dump();
    }

}

#endif // REALTIMESYSROVER_GUI_HELPER_HPP
