set(generatedDir "${CMAKE_BINARY_DIR}/generated")
file(MAKE_DIRECTORY ${generatedDir})

find_program(protoc NAMES protoc)

execute_process(
        COMMAND ${protoc}
        "-I=${CMAKE_CURRENT_SOURCE_DIR}"
        "--cpp_out=${generatedDir}"
        "--python_out=${CMAKE_SOURCE_DIR}/protobuf"
        "my_messages.proto"
)

add_library(proto_c "${generatedDir}/my_messages.pb.cc")
target_include_directories(proto_c PUBLIC ${generatedDir})
target_link_libraries(proto_c PUBLIC protobuf::libprotobuf)