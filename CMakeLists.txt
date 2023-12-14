cmake_minimum_required(VERSION 3.26)
project(RealtimeSysRover)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_definitions(-D_WIN32_WINNT=0x0601)

find_package(nlohmann_json CONFIG REQUIRED)
find_package(Boost REQUIRED COMPONENTS system) # beast and asio are header-only
find_package(Catch2 CONFIG REQUIRED)
find_package(Protobuf CONFIG REQUIRED)
find_package(OpenCV CONFIG REQUIRED)

set(RESOURCE_SOURCE_DIR "${CMAKE_SOURCE_DIR}/resources")
set(RESOURCE_DEST_DIR "${CMAKE_BINARY_DIR}/resources")
file(COPY ${RESOURCE_SOURCE_DIR}/ DESTINATION ${RESOURCE_DEST_DIR})

set(proto_dir "${CMAKE_BINARY_DIR}/generated")
file(MAKE_DIRECTORY ${proto_dir})
include_directories("${proto_dir}") # Not optimal, needed for generated protobuf files

add_subdirectory(resources/protobuf)
add_subdirectory(application)
add_subdirectory(src)
add_subdirectory(examples)

enable_testing()
add_subdirectory(tests)