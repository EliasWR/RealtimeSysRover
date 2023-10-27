
add_executable(GUI_server GUI_communication.cpp)
target_link_libraries(GUI_server tcp_server_lib )
target_include_directories(GUI_server PUBLIC ${Boost_INCLUDE_DIRS})

