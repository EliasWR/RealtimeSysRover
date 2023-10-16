
IF (Boost_FOUND)
    add_executable(GUI_server GUI_communication.cpp)
    target_include_directories(GUI_server PUBLIC ${Boost_INCLUDE_DIRS})
    target_link_libraries(GUI_server ${Boost_LIBRARIES})
ENDIF ()
