add_executable(main main.cpp
        ../tests/TestProject.cpp)

target_link_libraries(main PRIVATE Boost::system)