set(generatedDir "${CMAKE_BINARY_DIR}/generated")
file(MAKE_DIRECTORY ${generatedDir})

find_program(protoc NAMES protoc)

execute_process(
        COMMAND ${protoc}
        "-I=${CMAKE_CURRENT_SOURCE_DIR}"
        "--cpp_out=${generatedDir}"
        "${CMAKE_CURRENT_SOURCE_DIR}/Person.proto"
)

add_library(proto_msg "${generatedDir}/Person.pb.cc")
target_include_directories(proto_msg PUBLIC ${generatedDir})
target_link_libraries(proto_msg PUBLIC protobuf::libprotobuf)