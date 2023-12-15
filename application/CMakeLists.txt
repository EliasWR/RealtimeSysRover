# Autonomous driving example
add_executable(auto automated_driving.cpp)
target_link_libraries(auto PUBLIC
        udp_server_lib
        video_viewer_lib
        proto_c
        object_detection
        autonomous_driving
        tcp_server_lib)
target_include_directories(auto PUBLIC
        ${CMAKE_SOURCE_DIR}/include)

# Manual control with camera viewer
add_executable(manual manual_control_with_camera.cpp)
target_link_libraries(manual PUBLIC
        tcp_server_lib
        ws_server_lib
        video_viewer_lib
        proto_c
        udp_server_lib)
target_include_directories(manual PUBLIC
        ${CMAKE_SOURCE_DIR}/include)