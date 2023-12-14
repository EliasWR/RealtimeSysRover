find_program(protoc NAMES protoc)

execute_process(
        COMMAND $protoc}
        "-I=${CMAKE_CURRENT_SOURCE_DIR}"
        "--cpp_out=${proto_dir}"
        "--python_out=${CMAKE_CURRENT_SOURCE_DIR}"
        "my_messages.proto"
)

add_library(proto_c "${proto_dir}/my_messages.pb.cc")
target_include_directories(proto_c PUBLIC ${proto_dir})
target_link_libraries(proto_c PUBLIC protobuf::libprotobuf)