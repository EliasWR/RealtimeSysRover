cmake_minimum_required(VERSION 3.26)
project(RealtimeSysRover)

set(CMAKE_CXX_STANDARD 20)

add_definitions(-D_WIN32_WINNT=0x0601)

find_package(nlohmann_json CONFIG REQUIRED)
find_package(Boost REQUIRED COMPONENTS system) # beast and asio are header-only
find_package(Catch2 CONFIG REQUIRED)
find_package(Protobuf CONFIG REQUIRED)
find_package(OpenCV REQUIRED)

add_subdirectory(application)
add_subdirectory(src)
add_subdirectory(protobuf)
add_subdirectory(examples)
add_subdirectory(UDP)

enable_testing()
add_subdirectory(tests)