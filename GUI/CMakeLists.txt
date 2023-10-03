
IF (Boost_FOUND)
    add_executable(test_GUI test_GUI.cpp)
    include_directories(${Boost_INCLUDE_DIRS})
    target_link_libraries(test_GUI ${Boost_LIBRARIES})
ENDIF ()
