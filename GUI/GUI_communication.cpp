#include "GUI_communication.hpp"
#include <vector>

std::vector<std::string> splitString(const std::string &str, char delimiter) {
  std::vector<std::string> tokens;
  std::stringstream ss(str);
  std::string token;

  while (std::getline(ss, token, delimiter)) {
    tokens.push_back(token);
  }

  return tokens;
}

std::tuple<float_t, float_t> message_parser(const std::string& message) {
  std::vector<std::string> tokens = splitString(message, ',');

  auto command = tokens[0];

  if (command == "joystick"){
    if (tokens.size() != 4) {
      throw std::runtime_error("Invalid joystick message");
    } else {
      if (tokens[1] == "rover") {
        float_t x = std::stof(tokens[1]);
        float_t y = std::stof(tokens[2]);
        // TODO: Calulate speed and heading
        std::tuple<float_t, float_t> movement_instruction = {0, 0};
        return (movement_instruction);
      }

    }

  }

  float_t speed = std::stof(tokens[0]);
  float_t heading = std::stof(tokens[1]);
  return std::make_pair(speed, heading);
}


int main() {
  Server server(net::ip::make_address("0.0.0.0"), 12345);
  server.run();
  return 0;
}