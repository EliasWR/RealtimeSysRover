find_program(PROTOC NAMES protoc)

add_custom_command(
        OUTPUT "${proto_dir}/my_messages.pb.cc" "${proto_dir}/my_messages.pb.h"
        COMMAND "${PROTOC}"
        ARGS "-I${CMAKE_CURRENT_SOURCE_DIR}"
        "--cpp_out=${proto_dir}"
        "--python_out=${CMAKE_CURRENT_SOURCE_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}/my_messages.proto"
        DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/my_messages.proto"
        COMMENT "Running protoc"
)

add_custom_target(GenerateProtos ALL
        DEPENDS "${proto_dir}/my_messages.pb.cc" "${proto_dir}/my_messages.pb.h"
)

add_library(proto_c "${proto_dir}/my_messages.pb.cc")
add_dependencies(proto_c GenerateProtos)
target_include_directories(proto_c PUBLIC ${proto_dir})
target_link_libraries(proto_c PUBLIC protobuf::libprotobuf)