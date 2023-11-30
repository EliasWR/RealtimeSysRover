
#ifndef REALTIMESYSROVER_GUI_HELPER_HPP
#define REALTIMESYSROVER_GUI_HELPER_HPP


#include <numbers>
#include <iostream>

#include "nlohmann/json.hpp"

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

struct sCamera {
  int tilt;
  int pan;
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
  double magnitude = sqrt(pow(joy_x, 2) + pow(joy_y, 2));
  double angle = atan2(joy_y, joy_x);

  int heading = int(angle * 180 / std::numbers::pi_v<double>) - 90;

  if (heading < 0) {
    heading += 360;
  }

  heading = map(heading, 0, 360, 360, 0); // Flip the heading

  if (heading > 359) {
    heading -= 360;
  }

  int speed = magnitude * 255 / 100;



  return {heading, speed};
}

int direction_from_velocity(int value) {
  if (value > 0) {
    return 1;
  } else if (value < 0) {
    return 2;
  } else {
    return 0;
  }
}

sRawMotors joystick_to_raw_motors(int joy_x, int joy_y) {
  if (joy_y < 0) {
    joy_x = -joy_x;
  }

  int left_raw_velocity = joy_y + joy_x;
  int right_raw_velocity = joy_y - joy_x;

  int left_velocity = left_raw_velocity * 255 / 200;
  int right_velocity = right_raw_velocity * 255 / 200;

  int left_direction = direction_from_velocity(left_velocity);
  int right_direction = direction_from_velocity(right_velocity);

  return {left_direction, abs(left_velocity), right_direction, abs(right_velocity)};

}

sCamera joystick_to_camera(int joy_x, int joy_y) {
  std::cout << "joy_x: " << joy_x << std::endl;
  std::cout << "joy_y: " << joy_y << std::endl;

  int tilt = 35;
  int tilt_swing = 35;
  if (joy_x != 0) {
    tilt = map(joy_y, -100, 100, tilt - tilt_swing, tilt + tilt_swing);
  }
  int pan = 45;
  int pan_swing = 45;
  if (joy_y != 0) {
      pan = map(joy_x, -100, 100, pan + pan_swing, pan - pan_swing); // Pan is inverted
  }

  std::cout << "tilt: " << tilt << std::endl;
    std::cout << "pan: " << pan << std::endl;

  return {tilt, pan};
}


std::string message_handler(const std::string &message) {
  std::vector<std::string> tokens = splitString(message, '_');

  auto command = tokens[0];

  json rover_message = {
    {"command", ""}
  };

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
        auto p_and_t = joystick_to_camera(joy_x, joy_y);

        if(joy_x == 0 and joy_y == 0){
          rover_message["command"] = "reset_camera";
        }
        rover_message["servo_tilt"] = p_and_t.tilt;
        rover_message["servo_pan"] = p_and_t.pan;
      } else {
        std::cout << "Invalid joystick command" << std::endl;
      }

  } else {
    std::cout << "Invalid command" << std::endl;
    std::cout << "Message: " << message << std::endl;
  }

  rover_message["drive_mode"] = "tank";
  return rover_message.dump();
  std::cout << "Message: " << rover_message.dump(2) << std::endl;
}

#endif//REALTIMESYSROVER_GUI_HELPER_HPP
