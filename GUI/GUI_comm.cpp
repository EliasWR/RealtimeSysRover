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

std::pair<float_t, float_t> message_parser(const std::string& message) {
  std::vector<std::string> tokens = splitString(message, ',');

  auto component = tokens[0];
  auto command = tokens[1];
  auto value = tokens[2];

  if (component == "joystickrover"){

  }

  float_t speed = std::stof(tokens[0]);
  float_t heading = std::stof(tokens[1]);
  return std::make_pair(x, y);
}


int main() {
  Server server(net::ip::make_address("0.0.0.0"), 12345);
  server.run();
  return 0;
}