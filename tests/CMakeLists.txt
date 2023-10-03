#include_directories("${CMAKE_SOURCE_DIR}/include")
#include_directories("${CMAKE_SOURCE_DIR}/libraries")

find_package(catch2 CONFIG REQUIRED)

add_executable(testProject TestProject.cpp)
add_test(NAME testProject COMMAND testProject)