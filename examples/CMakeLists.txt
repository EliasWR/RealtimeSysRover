add_executable(picamera_server picamera_server.cpp)
target_link_libraries(picamera_server PUBLIC ${OpenCV_LIBRARIES} ${Boost_LIBRARIES})
target_include_directories(picamera_server PUBLIC
        ${OpenCV_INCLUDE_DIRS}
        ${Boost_INCLUDE_DIRS}
        ${CMAKE_CURRENT_SOURCE_DIR}/../include)
