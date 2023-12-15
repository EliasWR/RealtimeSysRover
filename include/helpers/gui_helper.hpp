#ifndef REALTIMESYSROVER_GUI_HELPER_HPP
#define REALTIMESYSROVER_GUI_HELPER_HPP

#include "nlohmann/json.hpp"
#include <cmath>
#include <iostream>
#include <numbers>
#include <sstream>

namespace GUI {

  constexpr int MAX_SPEED = 255;
  constexpr int MAX_HEADING = 360;
  constexpr int MIN_HEADING = 0;
  constexpr int MAX_TILT = 70;
  constexpr int MIN_TILT = 0;
  constexpr int MAX_PAN = 90;
  constexpr int MIN_PAN = 0;
  constexpr int TILT_ZERO = 35;
  constexpr int PAN_ZERO = 45;

  constexpr double PI = std::numbers::pi;
  constexpr int MAX_JOY_VAL = 100;

  using json = nlohmann::json;

    /**
     * @brief Maps a value from one range to another.
     *
     * This template function maps a value from its original range (in_min, in_max)
     * to a new range (out_min, out_max).
     *
     * @param val The value to be mapped.
     * @param in_min The minimum value of the input range.
     * @param in_max The maximum value of the input range.
     * @param out_min The minimum value of the output range.
     * @param out_max The maximum value of the output range.
     * @return The mapped value in the output range.
     */
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

  struct sCamera {
    int tilt;
    int pan;
  };

    /**
    * @brief Splits a string into tokens based on a delimiter.
    *
    * @param str The string to split.
    * @param delimiter The character used to split the string.
    * @return A vector of token strings.
    */
  std::vector<std::string> splitString(const std::string &str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delimiter)) {
      tokens.push_back(token);
    }
    return tokens;
  }
    /**
    * @brief Determines the direction based on velocity.
    *
    * @param value The velocity value.
    * @return The direction indicated by the velocity.
    */
  int direction_from_velocity(int value) {
    return (value > 0) ? 1 : (value < 0) ? 2 :
                                           0;
  }

    /**
     * @brief Converts joystick input to heading and speed for the rover.
     *
     * @param joy_x The x-coordinate of the joystick input.
     * @param joy_y The y-coordinate of the joystick input.
     * @return A structure containing the calculated heading and speed.
     */
  sMotors joystick_to_heading_and_speed(int joy_x, int joy_y) {
    double magnitude = std::hypot(joy_x, joy_y);

    int heading = static_cast<int>(std::atan2(joy_y, joy_x) * 180 / std::numbers::pi) - 90;
    heading = heading < 0 ? heading + MAX_HEADING : heading;
    heading = map(heading, MIN_HEADING, MAX_HEADING, MAX_HEADING, MIN_HEADING) % MAX_HEADING;// Flip the heading

    int speed = static_cast<int>(magnitude * MAX_SPEED / 100);

    return {heading, speed};
  }

    /**
    * @brief Converts joystick input to raw motor values for the rover.
    *
    * @param joy_x The x-coordinate of the joystick input.
    * @param joy_y The y-coordinate of the joystick input.
    * @return A structure containing the calculated raw motor commands.
    */
  sRawMotors joystick_to_raw_motors(int joy_x, int joy_y) {
    joy_x = joy_y < 0 ? -joy_x : joy_x; // Flip the x axis if the y axis is negative
    int left_velocity = (joy_y + joy_x) * MAX_SPEED / (MAX_JOY_VAL * 2);
    int right_velocity = (joy_y - joy_x) * MAX_SPEED / (MAX_JOY_VAL * 2);
    return {direction_from_velocity(left_velocity), std::abs(left_velocity),
            direction_from_velocity(right_velocity), std::abs(right_velocity)};
  }

    /**
     * @brief Converts joystick input to camera tilt and pan commands.
     *
     * @param joy_x The x-coordinate of the joystick input.
     * @param joy_y The y-coordinate of the joystick input.
     * @return A structure containing the camera tilt and pan values.
     */
  sCamera joystick_to_camera(int joy_x, int joy_y) {
    int tilt = joy_x == 0 ? TILT_ZERO : map(joy_x, -MAX_JOY_VAL, MAX_JOY_VAL, MIN_TILT, MAX_TILT);
    int pan = joy_y == 0 ? PAN_ZERO : map(joy_y, -MAX_JOY_VAL, MAX_JOY_VAL, MIN_PAN, MAX_PAN);

    return {tilt, pan};
  }

    /**
    * @brief Creates a rover message based on input commands.
    *
    * Parses the input commands and creates a JSON message to control the rover.
    * The message format and contents vary based on the command type.
    *
    * @param tokens A vector of strings representing the command tokens.
    * @return A JSON message for the rover.
    */
  json createRoverMessage(const std::vector<std::string> &tokens) {
    const auto &command = tokens[0];
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
      }
      else if (tokens[1] == "backward") {
        rover_message["left_direction"] = "2";
        rover_message["left_velocity"] = "255";
        rover_message["right_direction"] = "2";
        rover_message["right_velocity"] = "255";
      }
      else {
        std::cout << "Invalid move command" << std::endl;
      }
    }
    else if (command == "turn") {
      if (tokens[1] == "left") {
        rover_message["command"] = "move";
        rover_message["drive_mode"] = "tank";
        rover_message["left_direction"] = "1";
        rover_message["left_velocity"] = "80";
        rover_message["right_direction"] = "1";
        rover_message["right_velocity"] = "255";
      }
      else if (tokens[1] == "right") {
        rover_message["command"] = "move";
        rover_message["drive_mode"] = "tank";
        rover_message["left_direction"] = "1";
        rover_message["left_velocity"] = "255";
        rover_message["right_direction"] = "1";
        rover_message["right_velocity"] = "80";
      }
      else if (tokens[1] == "around") {
        if (tokens[2] == "left") {
          rover_message["command"] = "move";
          rover_message["drive_mode"] = "tank";
          rover_message["left_direction"] = "2";
          rover_message["left_velocity"] = "255";
          rover_message["right_direction"] = "1";
          rover_message["right_velocity"] = "255";
        }
        if (tokens[2] == "right") {
          rover_message["command"] = "move";
          rover_message["drive_mode"] = "tank";
          rover_message["left_direction"] = "1";
          rover_message["left_velocity"] = "255";
          rover_message["right_direction"] = "2";
          rover_message["right_velocity"] = "255";
        }
      }
      else {
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
        }
        else {

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
      }
      else if (tokens[1] == "camera") {
        int joy_x = std::stoi(tokens[2]);
        int joy_y = std::stoi(tokens[3]);
        auto p_and_t = joystick_to_camera(joy_x, joy_y);

        if (joy_x == 0 and joy_y == 0) {
          rover_message["command"] = "reset_camera";
        }
        rover_message["servo_tilt"] = p_and_t.tilt;
        rover_message["servo_pan"] = p_and_t.pan;
      }
      else {
        std::cout << "Invalid joystick command" << std::endl;
      }
    }
    else {
      std::cout << "Invalid command" << std::endl;
      for (auto &token : tokens) {
        std::cout << token << " ";
      }
      std::cout << std::endl;
    }

    /* Only tank-drive mode are used at the moment */
    rover_message["drive_mode"] = "tank";

    return rover_message;
  }

  std::string message_handler(const std::string &message) {
    std::vector<std::string> tokens = splitString(message, '_');
    json rover_message = createRoverMessage(tokens);
    return rover_message.dump();
  }
}// namespace GUI

#endif// REALTIMESYSROVER_GUI_HELPER_HPP
