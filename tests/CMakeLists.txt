#include_directories("${CMAKE_SOURCE_DIR}/include")
#include_directories("${CMAKE_SOURCE_DIR}/libraries")

add_executable(testProject testproject.cpp)
target_link_libraries(testProject PRIVATE Catch2::Catch2WithMain)
add_test(NAME testProject COMMAND testProject)