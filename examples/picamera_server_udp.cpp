// This file can be tested together with "RealtimeSystems\RealtimeSysRover\UDP\udp_client.py"

#include <iostream>

#include "udp_server/udp_server.hpp"
#include "message_handling/video_feed_handler.hpp"

std::unique_ptr<UDPServer> start_udp_server () {
    const int port = 8080;
    auto handler = std::make_unique<VideoFeedHandler>();
    auto server = std::make_unique<UDPServer>(port, std::move(handler));
    server->start();
    return server;
}

std::unique_ptr<UDPServer> start_udp_server_with_detection () {
    const int port = 8080;

    std::string model = "../../yolo/yolov3-tiny.weights";
    std::string config = "../../yolo/yolov3-tiny.cfg";

    auto handler = std::make_unique<VideoFeedHandler>(model, config);
    auto server = std::make_unique<UDPServer>(port, std::move(handler));
    server->start();
    return server;
}

int main() {
    // auto udp_server = start_udp_server();
    auto udp_server = start_udp_server_with_detection();

    std::cout << "Press a key + 'enter' to end..." << std::endl;
    while (std::cin.get() != '\n') {}
    std::cout << "Stopping camera feed" << std::endl;
}