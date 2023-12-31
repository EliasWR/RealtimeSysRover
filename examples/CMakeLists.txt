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
