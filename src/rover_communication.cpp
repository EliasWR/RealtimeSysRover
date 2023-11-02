#include "rover_communication.hpp"

int rover_communication(){
    auto server = TCPServer(9091);
    server.set_callback([](const std::string &msg, std::string &response){
        std::cout << "Message received: " << msg << std::endl;
        response = "I got , " + msg + "!\n";
    });

    server.run();
    std::cout << "Press a key + 'enter' to end..." << std::endl;
    while (std::cin.get() != '\n') {}

    return 0;
}