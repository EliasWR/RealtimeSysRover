cmake_minimum_required(VERSION 3.26)
project(RealtimeSysRover)

set(CMAKE_CXX_STANDARD 20)

add_definitions(-D_WIN32_WINNT=0x0601)

find_package(nlohmann_json CONFIG REQUIRED)
find_package(Boost REQUIRED COMPONENTS system)

add_subdirectory(application)
add_subdirectory(GUI)