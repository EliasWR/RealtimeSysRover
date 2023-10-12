cmake_minimum_required(VERSION 3.26)
project(RealtimeSysRover)

set(CMAKE_CXX_STANDARD 20)

find_package(Boost REQUIRED COMPONENTS system)
find_package(Catch2 CONFIG REQUIRED)
find_package(protobuf CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

add_subdirectory(application)
add_subdirectory(protobuf)

enable_testing()
add_subdirectory(tests)