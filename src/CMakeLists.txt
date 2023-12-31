# Video viewer
add_library(video_viewer_lib realtime_sys_rover/video_viewer/video_viewer.cpp)
target_include_directories(video_viewer_lib PUBLIC
        "${OpenCV_INCLUDE_DIRS}"
        "${CMAKE_SOURCE_DIR}/include")
target_link_libraries(video_viewer_lib PUBLIC
        ${OpenCV_LIBS})

# Object Detection
add_library(object_detection realtime_sys_rover/object_detection/object_detection.cpp)
target_include_directories(object_detection PUBLIC
        "${OpenCV_INCLUDE_DIRS}"
        "${CMAKE_SOURCE_DIR}/include"
        "${CMAKE_SOURCE_DIR}/resources")

# TCP Server
add_library(tcp_server_lib realtime_sys_rover/tcp_server/tcp_server_lib.cpp)
target_include_directories(tcp_server_lib PUBLIC
        "${BOOST_INCLUDE_DIRS}"
        "${CMAKE_SOURCE_DIR}/include")
target_link_libraries(tcp_server_lib PUBLIC
        Boost::system)

# WebSockets server
add_library(ws_server_lib realtime_sys_rover/tcp_server/ws_server_lib.cpp)
target_include_directories(ws_server_lib PUBLIC
        "${BOOST_INCLUDE_DIRS}"
        "${CMAKE_SOURCE_DIR}/include")
target_link_libraries(ws_server_lib PUBLIC
        Boost::system)

# udp server
add_library(udp_server_lib realtime_sys_rover/udp_server/udp_server_lib.cpp)
target_include_directories(udp_server_lib PUBLIC
        "${BOOST_INCLUDE_DIRS}"
        "${CMAKE_SOURCE_DIR}/include"
        "${CMAKE_BINARY_DIR}/generated")
target_link_libraries(udp_server_lib PUBLIC
        Boost::system
        proto_c)

# Autonomous driving
add_library(autonomous_driving realtime_sys_rover/autonomous_driving/autonomous_driving.cpp)
target_include_directories(autonomous_driving PUBLIC
        "${OpenCV_INCLUDE_DIRS}"
        "${CMAKE_SOURCE_DIR}/include")

if (UNIX)
    target_link_libraries(tcp_server_lib PUBLIC pthread)
    target_link_libraries(ws_server_lib PUBLIC pthread)
    target_link_libraries(udp_server_lib PUBLIC pthread)
    target_link_libraries(object_detection PUBLIC pthread)
    target_link_libraries(autonomous_driving PUBLIC pthread)
endif ()