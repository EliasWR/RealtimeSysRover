ADD_LIBRARY(tcp_server tcp_server.cpp)
TARGET_LINK_LIBRARIES(tcp_server PUBLIC ${Boost_LIBRARIES})
TARGET_INCLUDE_DIRECTORIES(tcp_server PUBLIC
        ${Boost_INCLUDE_DIRS},
        ${CMAKE_SOURCE_DIR}/include)


ADD_LIBRARY(tcp_server_lib STATIC _tcp_server.cpp)
TARGET_LINK_LIBRARIES(tcp_server_lib PUBLIC Boost::system)
TARGET_INCLUDE_DIRECTORIES(tcp_server_lib PUBLIC
        "${BOOST_INCLUDE_DIRS}"
        "${CMAKE_SOURCE_DIR}/include")



add_executable(test_tcp_NO_CATCH test_tcp_NO_CATCH.cpp)
target_link_libraries(test_tcp_NO_CATCH tcp_server_lib)


