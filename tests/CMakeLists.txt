function(add_test_executable name)
    add_executable(${name} ${name}.cpp)
    target_link_libraries(${name} PRIVATE Catch2::Catch2WithMain)
    target_include_directories(${name} PRIVATE "${PROJECT_SOURCE_DIR}/src")
    add_test(NAME ${name} COMMAND ${name})
endfunction()

add_test_executable(testProject)
add_test_executable(testSerialization)

# PROTOBUF
target_link_libraries(testSerialization PRIVATE protobuf::libprotobuf)

target_include_directories(testSerialization PRIVATE "${CMAKE_SOURCE_DIR}/protobuf")
target_link_libraries(testSerialization PRIVATE
        proto_msg
        nlohmann_json::nlohmann_json
)

#add_executable(testProject testproject.cpp)
#target_link_libraries(testProject PRIVATE Catch2::Catch2WithMain)
#add_test(NAME testProject COMMAND testProject)

#add_executable(testSerialization testSerialization.cpp)
#target_link_libraries(testSerialization PRIVATE Catch2::Catch2WithMain)
#add_test(NAME testSerialization COMMAND testSerialization)