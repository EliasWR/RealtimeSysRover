#include <iostream>
#include "udp_server/udp_server.hpp"
#include "message_handling/message_handler.hpp"
#include "udp_server/video_feed_handler.hpp"

std::unique_ptr<UDPServer<VideoFeedHandler>> start_udp_server () {
    const int port = 8080;
    auto handler = std::make_unique<VideoFeedHandler>();
    auto server = std::make_unique<UDPServer<VideoFeedHandler>>(port, std::move(handler));
    server->start();
    return server;
}

int main() {
    auto udp_server = start_udp_server();

    std::cout << "Press a key + 'enter' to end..." << std::endl;
    while (std::cin.get() != '\n') {}
    std::cout << "Stopping camera feed" << std::endl;
}