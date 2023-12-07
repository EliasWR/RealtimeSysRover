# Picamera server tcp
add_executable(picamera_server_tcp executables/picamera_server.cpp)
target_link_libraries(picamera_server_tcp PUBLIC ${OpenCV_LIBRARIES} ${Boost_LIBRARIES})
target_include_directories(picamera_server_tcp PUBLIC
        ${OpenCV_INCLUDE_DIRS}
        ${Boost_INCLUDE_DIRS}
        ${CMAKE_SOURCE_DIR}/include)

# Picamera server udp
add_executable(picamera_server_udp executables/picamera_server_udp.cpp)
target_link_libraries(picamera_server_udp PUBLIC
        udp_server_lib
        video_viewer_lib
        proto_c
        object_detection)
target_include_directories(picamera_server_udp PUBLIC
        "${CMAKE_BINARY_DIR}/generated")

# Manual control
add_executable(manual_control executables/manual_control.cpp)
target_link_libraries(manual_control PUBLIC
        tcp_server_lib
        ws_server_lib)
target_include_directories(manual_control PUBLIC
        ${CMAKE_SOURCE_DIR}/include)

# Video viewer
add_executable(video_view executables/video_viewer_example.cpp)
target_link_libraries(video_view PUBLIC
        video_viewer_lib)
target_include_directories(video_view PUBLIC
        ${CMAKE_SOURCE_DIR}/include)

# Object detection threading
add_executable(object_detection_threading executables/object_detection_threading.cpp)
target_link_libraries(object_detection_threading PUBLIC
        udp_server_lib
        video_viewer_lib
        proto_c
        object_detection)
target_include_directories(object_detection_threading PUBLIC
        "${CMAKE_BINARY_DIR}/generated")

# Autonomous driving commands
add_executable(automated_driving_command executables/automated_driving_command.cpp)
target_link_libraries(automated_driving_command PUBLIC
        udp_server_lib
        video_viewer_lib
        proto_c
        object_detection
        autonomous_driving)
target_include_directories(automated_driving_command PUBLIC
        "${CMAKE_BINARY_DIR}/generated")

# Autonomous driving example
add_executable(automated_driving executables/automated_driving.cpp)
target_link_libraries(automated_driving PUBLIC
        udp_server_lib
        video_viewer_lib
        proto_c
        object_detection
        autonomous_driving
        tcp_server_lib)
target_include_directories(automated_driving PUBLIC
        "${CMAKE_BINARY_DIR}/generated")

# Manual control with camera viewer
add_executable(manual_control_with_camera executables/manual_control_with_camera.cpp)
target_link_libraries(manual_control_with_camera PUBLIC
        tcp_server_lib
        ws_server_lib
        video_viewer_lib
        proto_c
        udp_server_lib)
target_include_directories(manual_control_with_camera PUBLIC
        ${CMAKE_SOURCE_DIR}/include
        "${CMAKE_BINARY_DIR}/generated")