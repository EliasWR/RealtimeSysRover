#ifndef REALTIMESYSROVER_UTILS_HPP
#define REALTIMESYSROVER_UTILS_HPP

#include <iostream>
#include <vector>
#include <sstream>

namespace util {

  template<typename T>
  inline int map(T val, T in_min, T in_max, T out_min, T out_max) {
    return (val - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
  }

  inline std::vector<std::string> splitString(const std::string &str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;

    while (std::getline(ss, token, delimiter)) {
      tokens.push_back(token);
    }

    return tokens;
  }


}
#endif//REALTIMESYSROVER_UTILS_HPP
