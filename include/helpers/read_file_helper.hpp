#ifndef REALTIMESYSROVER_READFILEHELPER_H
#define REALTIMESYSROVER_READFILEHELPER_H

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

/**
 * @brief Loads the contents of a file into a vector of strings.
 *
 * This function opens the specified file and reads its contents line by line.
 * Each line is stored as an element in a vector of strings.
 *
 * @param filename The name (and path) of the file to be loaded.
 * @return A vector containing the lines of the file as strings.
 */
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
