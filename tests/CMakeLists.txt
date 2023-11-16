function(add_test_executable name)
    add_executable(${name} ${name}.cpp)
    target_link_libraries(${name} PRIVATE Catch2::Catch2WithMain)
    target_include_directories(${name} PRIVATE "${CMAKE_SOURCE_DIR}/include")
    add_test(NAME ${name} COMMAND ${name})
endfunction()

add_test_executable(test_project)
add_test_executable(test_serialization)
add_test_executable(test_gui_helper)
#add_test_executable(test_tcp)

# PROTOBUF
target_link_libraries(test_serialization PRIVATE protobuf::libprotobuf)

target_include_directories(test_serialization PRIVATE "${CMAKE_SOURCE_DIR}/protobuf")
target_link_libraries(test_serialization PRIVATE
        nlohmann_json::nlohmann_json
)



target_include_directories(test_gui_helper PRIVATE "${CMAKE_SOURCE_DIR}/GUI")


#add_executable(testProject testproject.cpp)
#target_link_libraries(testProject PRIVATE Catch2::Catch2WithMain)
#add_test(NAME testProject COMMAND testProject)

#add_executable(testSerialization testSerialization.cpp)
#target_link_libraries(testSerialization PRIVATE Catch2::Catch2WithMain)
#add_test(NAME testSerialization COMMAND testSerialization)