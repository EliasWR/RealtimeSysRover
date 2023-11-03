add_executable(picamera_server_tcp picamera_server.cpp)
target_link_libraries(picamera_server_tcp PUBLIC ${OpenCV_LIBRARIES} ${Boost_LIBRARIES})
target_include_directories(picamera_server_tcp PUBLIC
        ${OpenCV_INCLUDE_DIRS}
        ${Boost_INCLUDE_DIRS}
        ${CMAKE_SOURCE_DIR}/include)

add_executable(picamera_server_udp picamera_server_udp.cpp)
target_link_libraries(picamera_server_udp PUBLIC ${OpenCV_LIBRARIES} ${Boost_LIBRARIES})
target_include_directories(picamera_server_udp PUBLIC
        ${OpenCV_INCLUDE_DIRS}
        ${Boost_INCLUDE_DIRS}
        ${CMAKE_SOURCE_DIR}/include)


add_executable(manual_control manual_control.cpp)
target_link_libraries(manual_control PUBLIC tcp_server_lib)
target_include_directories(manual_control PUBLIC
        ${CMAKE_SOURCE_DIR}/include)