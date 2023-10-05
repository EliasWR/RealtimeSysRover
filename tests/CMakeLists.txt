#include_directories("${CMAKE_SOURCE_DIR}/include")
#include_directories("${CMAKE_SOURCE_DIR}/libraries")

find_package(Catch2 CONFIG REQUIRED)

add_executable(testProject TestProject.cpp)
target_link_libraries(testProject PUBLIC Catch2::Catch2WithMain)
add_test(NAME TestTest COMMAND TestProject)