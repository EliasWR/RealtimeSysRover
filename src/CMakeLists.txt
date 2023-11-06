ADD_LIBRARY(tcp_server_lib STATIC tcp_server.cpp)
TARGET_LINK_LIBRARIES(tcp_server_lib PUBLIC ${Boost_LIBRARIES})
TARGET_INCLUDE_DIRECTORIES(tcp_server_lib PUBLIC
        "${BOOST_INCLUDE_DIRS}"
        "${CMAKE_SOURCE_DIR}/include")

IF (UNIX)
    TARGET_LINK_LIBRARIES(tcp_server_lib PUBLIC pthread)
ENDIF ()





