
add_executable(GUI_server GUI_communication.cpp)
target_include_directories(GUI_server PRIVATE
        ${Boost_INCLUDE_DIRS}
        ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(GUI_server ${Boost_LIBRARIES})

