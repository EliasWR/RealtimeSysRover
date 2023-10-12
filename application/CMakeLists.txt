add_executable(main main.cpp
        ../src/proto_helper.hpp)
target_link_libraries(main PRIVATE Boost::system)
target_link_libraries(main PRIVATE protobuf::libprotobuf)

#add_subdirectory(../src)
target_include_directories(main PRIVATE "${CMAKE_SOURCE_DIR}/src")
target_include_directories(main PRIVATE "${CMAKE_SOURCE_DIR}/protobuf")
target_link_libraries(main PRIVATE
        proto_msg
        nlohmann_json::nlohmann_json
)