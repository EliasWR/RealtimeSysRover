add_executable(main main.cpp)

target_include_directories(main PRIVATE
        "${CMAKE_SOURCE_DIR}/protobuf"
        "${CMAKE_SOURCE_DIR}/include")

target_link_libraries(main PRIVATE
        Boost::system
        protobuf::libprotobuf
        proto_msg
        nlohmann_json::nlohmann_json)