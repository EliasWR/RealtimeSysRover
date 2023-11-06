function(setup_target target_name)
    target_include_directories(${target_name} PUBLIC
            "${BOOST_INCLUDE_DIRS}"
            "${CMAKE_SOURCE_DIR}/include")
    target_link_libraries(${target_name} PUBLIC Boost::system)
    if (UNIX)
        target_link_libraries(${target_name} PUBLIC pthread)
    endif ()
endfunction()

set(targets tcp_server_lib udp_server_lib)

foreach(target ${targets})
    add_library(${target} STATIC ${target}.cpp)
    setup_target(${target})

    # If additional configuration is needed for the udp_server_lib specifically
    if(${target} STREQUAL "udp_server_lib")
        target_include_directories(${target} PRIVATE proto_c)
        target_link_libraries(${target} PRIVATE ${OpenCV_LIBS})
    endif()
endforeach()
