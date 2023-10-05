cmake_minimum_required(VERSION 3.26)
project(RealtimeSysRover)

set(CMAKE_CXX_STANDARD 20)

find_package(Boost REQUIRED COMPONENTS system)
find_package(Catch2 CONFIG REQUIRED)

add_subdirectory(application)
add_subdirectory(tests)