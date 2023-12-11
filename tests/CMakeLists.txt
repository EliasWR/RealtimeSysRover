function(add_test_executable name)
    add_executable(${name} ${name}.cpp)
    target_link_libraries(${name} PRIVATE Catch2::Catch2WithMain)
    target_include_directories(${name} PRIVATE "${CMAKE_SOURCE_DIR}/include")
    add_test(NAME ${name} COMMAND ${name})
endfunction()

add_test_executable(test_project)

add_subdirectory(test_headers)
add_subdirectory(test_helpers)