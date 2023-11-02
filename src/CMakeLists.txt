ADD_LIBRARY(tcp_server_lib STATIC tcp_server.cpp)
#TARGET_LINK_LIBRARIES(tcp_server_lib PUBLIC ${Boost_LIBRARIES})
TARGET_LINK_LIBRARIES(tcp_server_lib PUBLIC Boost::system)
TARGET_INCLUDE_DIRECTORIES(tcp_server_lib PUBLIC
        "${BOOST_INCLUDE_DIRS}"
        "${CMAKE_SOURCE_DIR}/include")


ADD_LIBRARY(ws_tcp_server_lib STATIC websocket_tcp_server.cpp)
#TARGET_LINK_LIBRARIES(tcp_server_lib PUBLIC ${Boost_LIBRARIES})
TARGET_LINK_LIBRARIES(ws_tcp_server_lib PUBLIC Boost::system)
TARGET_INCLUDE_DIRECTORIES(ws_tcp_server_lib PUBLIC
        "${BOOST_INCLUDE_DIRS}"
        "${CMAKE_SOURCE_DIR}/include")



add_executable(test_tcp_NO_CATCH test_tcp_NO_CATCH.cpp)
target_link_libraries(test_tcp_NO_CATCH tcp_server_lib)

add_executable(communication rover_communication.cpp)
target_include_directories(communication PUBLIC
        "${CMAKE_SOURCE_DIR}/include")
