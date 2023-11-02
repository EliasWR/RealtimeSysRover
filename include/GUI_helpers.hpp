
#ifndef REALTIMESYSROVER_GUI_HELPERS_HPP
#define REALTIMESYSROVER_GUI_HELPERS_HPP

#define _USE_MATH_DEFINES


int map(int val, int in_min, int in_max, int out_min, int out_max);
#include <cmath>
#include <iostream>

#include "nlohmann/json.hpp"

using json = nlohmann::json;

std::vector<std::string> splitString(const std::string &str, char delimiter) {
  std::vector<std::string> tokens;
  std::stringstream ss(str);
  std::string token;

  while (std::getline(ss, token, delimiter)) {
    tokens.push_back(token);
  }

  return tokens;
}


int get_direction(int value){
  if (value > 0){
    return 1;
  }
  else if (value < 0){
    return 2;
  }
  else{
    return 0;
  }
}

std::string joystick_to_heading_and_speed(int joy_x, int joy_y){
  auto magnitude = sqrt(pow(joy_x, 2) + pow(joy_y, 2));
  auto angle = atan2(joy_y, joy_x);

  // PI = atan(1)*4
  int heading = int(angle * 180 / (atan(1)*4 ));
  heading = map(heading, -180, 180, 0, 360);
  int speed = magnitude * 255 / 100;

  return std::to_string(heading) + "_" + std::to_string(speed);
}
int map(int val, int in_min, int in_max, int out_min, int out_max)
{
  return (val - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

std::string joystick_to_raw_motors(int joy_x, int joy_y){

    int left_raw_velocity = joy_y + joy_x;
    int right_raw_velocity = joy_y - joy_x;

    int left_velocity = left_raw_velocity * 255 / 100;
    int right_velocity = right_raw_velocity * 255 / 100;

    int left_direction = get_direction(left_velocity);
    int right_direction = get_direction(right_velocity);

    std::string left_motor = std::to_string(left_direction) + "_" +  std::to_string(abs(left_velocity));
    std::string right_motor = std::to_string(right_direction) + "_" + std::to_string(abs(right_velocity));

    return left_motor + "_" + right_motor;
}



void message_handler(const std::string &message) {
    std::vector<std::string> tokens = splitString(message, '_');

    auto command = tokens[ 0 ];

    json rover_message;

    if (command == "stop"){
    std::cout << "Stopping" << std::endl;
    rover_message["command"] = "stop";
    rover_message["left_motor"] = "0_0";
    rover_message["right_motor"] = "0_0";
    }
    else if (command == "move") {
    if (tokens.size() != 2) {
      throw std::runtime_error("Invalid move message");
    } else {
      rover_message["command"] = "move";
      if (tokens[ 1 ] == "forward") {
        std::cout << "Moving forward" << std::endl;
        rover_message["heading"] = "0";
        rover_message["speed"] = "255";
      } else if (tokens[ 1 ] == "backward") {
        std::cout << "Moving backward" << std::endl;
        rover_message["heading"] = "180";
        rover_message["speed"] = "255";
      } else if (tokens[ 1 ] == "left") {
        std::cout << "Moving left" << std::endl;
        rover_message["heading"] = "270";
        rover_message["speed"] = "255";
      } else if (tokens[ 1 ] == "right") {
        std::cout << "Moving right" << std::endl;
        rover_message["heading"] = "90";
        rover_message["speed"] = "255";
      } else {
        std::cout << "Invalid move command" << std::endl;
      }
    }
    }
    else if (command == "joystick"){
      if (tokens.size() != 4){
        throw std::runtime_error("Invalid joystick message");
      }
      else {
        if (tokens[ 1 ] == "rover") {
          rover_message["command"] = "move";
          int joy_x = std::stoi(tokens[ 2 ]);
          int joy_y = std::stoi(tokens[ 3 ]);
          auto h_and_s = joystick_to_heading_and_speed(joy_x, joy_y);
          auto as_vector = splitString(h_and_s, '_');
          rover_message["heading"] = as_vector[ 0 ];
          rover_message["speed"] = as_vector[ 1 ];
        }
        else if(tokens[ 1 ] == "camera"){
          int joy_x = std::stoi(tokens[ 2 ]);
          int joy_y = std::stoi(tokens[ 3 ]);
          std::cout << "Joystick: " << joy_x << ", " << joy_y << std::endl;
          std::cout << "Camera: " << joystick_to_heading_and_speed(joy_x, joy_y) << std::endl;
        }
        else {
          std::cout << "Invalid joystick command" << std::endl;
        }
      }


    }
    else {
        std::cout << "Invalid command" << std::endl;
        std::cout << "Message: " << message << std::endl;
    }

    std::cout << "Message: " << rover_message.dump(2) << std::endl;
}

#endif//REALTIMESYSROVER_GUI_HELPERS_HPP
