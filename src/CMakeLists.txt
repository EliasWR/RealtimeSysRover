add_library(video_feed_handler STATIC video_feed_handler.cpp)
add_library(tcp_server_lib STATIC tcp_server_lib.cpp)
add_library(udp_server_lib STATIC udp_server_lib.cpp)

# Video Feed Handler
target_include_directories(video_feed_handler PUBLIC
        "${BOOST_INCLUDE_DIRS}"
        "${CMAKE_SOURCE_DIR}/include"
        "${CMAKE_BINARY_DIR}/generated"
        proto_c)
target_link_libraries(video_feed_handler PRIVATE ${OpenCV_LIBS})
target_link_libraries(video_feed_handler PRIVATE proto_c)

# TCP server
target_include_directories(tcp_server_lib PUBLIC
        "${BOOST_INCLUDE_DIRS}"
        "${CMAKE_SOURCE_DIR}/include")
target_link_libraries(tcp_server_lib PUBLIC Boost::system)

# UDP server
target_include_directories(udp_server_lib PUBLIC
        "${BOOST_INCLUDE_DIRS}"
        "${CMAKE_SOURCE_DIR}/include"
        "${CMAKE_BINARY_DIR}/generated"
        proto_c)
target_link_libraries(udp_server_lib PUBLIC Boost::system)
target_link_libraries(udp_server_lib PRIVATE proto_c)
target_link_libraries(udp_server_lib PRIVATE ${OpenCV_LIBS})
target_link_libraries(udp_server_lib PUBLIC video_feed_handler)

if (UNIX)
    target_link_libraries(tcp_server_lib PUBLIC pthread)
    target_link_libraries(udp_server_lib PUBLIC pthread)
endif ()

