# Video Feed Handler
add_library(video_feed_handler video_feed_handler.cpp)
target_include_directories(video_feed_handler PUBLIC
        "${CMAKE_SOURCE_DIR}/include"
        "${CMAKE_BINARY_DIR}/generated"
        proto_c)
target_link_libraries(video_feed_handler PUBLIC
        ${OpenCV_LIBS}
        proto_c)

# Object Detection
add_library(object_detection object_detection.cpp)
target_include_directories(object_detection PUBLIC
        "${CMAKE_SOURCE_DIR}/include"
        "${CMAKE_SOURCE_DIR}/yolo")
target_link_libraries(object_detection PUBLIC
        ${OpenCV_LIBS})

# TCP Server
add_library(tcp_server_lib tcp_server_lib.cpp)
target_include_directories(tcp_server_lib PUBLIC
        "${BOOST_INCLUDE_DIRS}"
        "${CMAKE_SOURCE_DIR}/include")
target_link_libraries(tcp_server_lib PUBLIC
        Boost::system)

# WebSockets server
add_library(ws_server_lib ws_server_lib.cpp)
target_include_directories(ws_server_lib PUBLIC
        "${BOOST_INCLUDE_DIRS}"
        "${CMAKE_SOURCE_DIR}/include")
target_link_libraries(ws_server_lib PUBLIC Boost::system)

# UDP server
add_library(udp_server_lib udp_server_lib.cpp)
target_include_directories(udp_server_lib PUBLIC
        "${BOOST_INCLUDE_DIRS}"
        "${CMAKE_SOURCE_DIR}/include"
        "${CMAKE_BINARY_DIR}/generated"
        proto_c)
target_link_libraries(udp_server_lib PUBLIC
        Boost::system
        proto_c
        ${OpenCV_LIBS}
        video_feed_handler)

if (UNIX)
    target_link_libraries(tcp_server_lib PUBLIC pthread)
    target_link_libraries(ws_server_lib PUBLIC pthread)
    target_link_libraries(udp_server_lib PUBLIC pthread)
endif ()