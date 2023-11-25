add_executable(picamera_server_tcp picamera_server.cpp)
target_link_libraries(picamera_server_tcp PUBLIC ${OpenCV_LIBRARIES} ${Boost_LIBRARIES})
target_include_directories(picamera_server_tcp PUBLIC
        ${OpenCV_INCLUDE_DIRS}
        ${Boost_INCLUDE_DIRS}
        ${CMAKE_SOURCE_DIR}/include)

add_executable(picamera_server_udp picamera_server_udp.cpp)
target_link_libraries(picamera_server_udp PUBLIC
        udp_server_lib
        video_viewer_lib
        proto_c
        "${CMAKE_BINARY_DIR}/generated"
        object_detection )

add_executable(manual_control manual_control.cpp)
target_link_libraries(manual_control PUBLIC
        tcp_server_lib
        ws_server_lib)
target_include_directories(manual_control PUBLIC
        ${CMAKE_SOURCE_DIR}/include)



add_executable(video_view video_viewer_example.cpp)
target_link_libraries(video_view PUBLIC
        video_viewer_lib)
target_include_directories(video_view PUBLIC
        ${CMAKE_SOURCE_DIR}/include)

add_executable(automated_control automated_control.cpp)