add_executable(udp_server udp_server.cpp)
include_directories(proto_c)
target_link_libraries(udp_server PRIVATE Boost::system proto_c)

