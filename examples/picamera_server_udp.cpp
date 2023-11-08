#include <iostream>
#include "udp_server/udp_server.hpp"

std::unique_ptr<UDPServer> start_udp_server () {
    const int port = 8080;
    auto server = std::make_unique<UDPServer>(port);
    server->start();
    return server;
}

int main() {
    auto udp_server = start_udp_server();

    std::cout << "Press a key + 'enter' to end..." << std::endl;
    while (std::cin.get() != '\n') {
    }
    std::cout << "Stopping..." << std::endl;
    udp_server->~UDPServer();
}