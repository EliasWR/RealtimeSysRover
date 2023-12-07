#ifndef REALTIMESYSROVER_READFILEHELPER_H
#define REALTIMESYSROVER_READFILEHELPER_H

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

inline std::vector<std::string> loadFileToVector(const std::string &filename) {
  std::vector<std::string> lines;
  std::ifstream file;
  file.open(filename);

  if (!file.is_open()) {
    std::cerr << "Failed to open " << filename << std::endl;
    return lines;
  }
  std::string line;
  while (std::getline(file, line)) {
    lines.push_back(line);
  }
  file.close();
  return lines;
}

#endif//REALTIMESYSROVER_READFILEHELPER_H
