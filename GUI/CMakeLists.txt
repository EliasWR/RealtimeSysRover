
IF (Boost_FOUND)
    add_executable(test_GUI GUI_communication.cpp)
    include_directories(${Boost_INCLUDE_DIRS})
    target_link_libraries(test_GUI ${Boost_LIBRARIES})

    add_executable(GUI_server GUI_comm.cpp)
    target_include_directories(GUI_server PUBLIC ${Boost_INCLUDE_DIRS})
    target_link_libraries(GUI_server ${Boost_LIBRARIES})
ENDIF ()
