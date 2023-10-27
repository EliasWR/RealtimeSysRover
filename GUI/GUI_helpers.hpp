
#ifndef REALTIMESYSROVER_GUI_HELPERS_HPP
#define REALTIMESYSROVER_GUI_HELPERS_HPP

#define _USE_MATH_DEFINES


#include <cmath>
#include <iostream>

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


  int heading = angle * 180 / (atan(1)*4 ) - 90;
  int speed = magnitude * 255 / 100;

  return std::to_string(heading) + "_" + std::to_string(speed);
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
    std::for_each(tokens.begin(), tokens.end(), [](const std::string &token) {
      std::cout << token << std::endl;
    });
    auto command = tokens[ 0 ];

    if (command == "move") {
    if (tokens.size() != 2) {
      throw std::runtime_error("Invalid move message");
    } else {
      if (tokens[ 1 ] == "forward") {
        std::cout << "Moving forward" << std::endl;
      } else if (tokens[ 1 ] == "backward") {
        std::cout << "Moving backward" << std::endl;
      } else if (tokens[ 1 ] == "left") {
        std::cout << "Moving left" << std::endl;
      } else if (tokens[ 1 ] == "right") {
        std::cout << "Moving right" << std::endl;
      } else {
        std::cout << "Invalid move command" << std::endl;
      }
    }
    }
    else if (command == "stop"){
    std::cout << "Stopping" << std::endl;
    }
    else if (command == "joystick"){
      if (tokens.size() != 4){
        throw std::runtime_error("Invalid joystick message");
      }
      else {
        if (tokens[ 1 ] == "rover") {
          int joy_x = std::stoi(tokens[ 1 ]);
          int joy_y = std::stoi(tokens[ 2 ]);
          std::cout << "Joystick: " << joy_x << ", " << joy_y << std::endl;
          std::cout << "Raw motors: " << joystick_to_raw_motors(joy_x, joy_y) << std::endl;
        }
      }
    }
}

#endif//REALTIMESYSROVER_GUI_HELPERS_HPP
