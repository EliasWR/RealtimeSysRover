function(add_test_executable name)
    add_executable(${name} ${name}.cpp)
    target_link_libraries(${name} PRIVATE Catch2::Catch2WithMain)
    target_include_directories(${name} PRIVATE "${CMAKE_SOURCE_DIR}/include")
    add_test(NAME ${name} COMMAND ${name})
endfunction()

add_test_executable(test_project)
add_test_executable(test_serialization)
add_test_executable(test_gui_helper)

target_link_libraries(test_serialization PUBLIC
        protobuf::libprotobuf
        nlohmann_json::nlohmann_json)
target_include_directories(test_serialization PRIVATE "${CMAKE_SOURCE_DIR}/protobuf")

target_include_directories(test_gui_helper PRIVATE "${CMAKE_SOURCE_DIR}/GUI")