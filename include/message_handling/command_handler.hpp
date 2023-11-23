#ifndef REALTIMESYSROVER_COMMAND_HANDLER_HPP
#define REALTIMESYSROVER_COMMAND_HANDLER_HPP

#include "nlohmann/json.hpp"
#include <iostream>
#include <numbers>

#include "message_handling/message_handler.hpp"
#include "utils.hpp"

using json = nlohmann::json;

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


class CommandHandler : public MessageHandler {
public:
  CommandHandler () = default;

  void handleMessage(const std::vector<char>& message, std::vector<char>& response, size_t len) override;

private:
  static sMotors joystick_to_heading_and_speed(int joy_x, int joy_y);
  static int direction_from_velocity(int value);
  static sRawMotors joystick_to_raw_motors(int joy_x, int joy_y);

};


#endif//REALTIMESYSROVER_COMMAND_HANDLER_HPP
