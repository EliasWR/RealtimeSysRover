# Inspired by Networking demo written by Lars Ivar Hatledal
# https://github.com/AIS2203-H-2023/serialization_demo/tree/master/protobuf

find_program(PROTOC NAMES protoc)

execute_process(
        COMMAND ${protoc}
        "-I=${CMAKE_CURRENT_SOURCE_DIR}"
        "--cpp_out=${proto_dir}"
        "--python_out=${CMAKE_CURRENT_SOURCE_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}/Person.proto"
)

add_library(proto_c "${proto_dir}/my_messages.pb.cc")
target_include_directories(proto_c PUBLIC ${proto_dir})
target_link_libraries(proto_c PUBLIC protobuf::libprotobuf)