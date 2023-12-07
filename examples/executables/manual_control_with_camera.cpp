#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "helpers/gui_helper.hpp"
#include "safe_queue/safe_queue.hpp"
#include "tcp_server/tcp_server_lib.hpp"
#include "tcp_server/ws_server_lib.hpp"
#include "video_viewer/video_viewer.hpp"
#include "udp_server/udp_server.hpp"
#include "my_messages.pb.h"

#include <iostream>

cv::Mat decodeImageFromProto (const std::string& frame) {
    VideoFeed video_feed;
    video_feed.ParseFromString(frame);

    std::vector<uchar> encoded_frame(video_feed.messagefeed().begin(), video_feed.messagefeed().end());
    cv::Mat decoded_frame = cv::imdecode(encoded_frame, cv::IMREAD_COLOR);
    return decoded_frame;
}

int main() {
    SafeQueue<std::string> command_queue;
    std::atomic<bool> stop{false};

    auto Viewer = std::make_unique<VideoViewer>("Camera View");


    auto WebsocketServer = WSServer(12345);
    WebsocketServer.setMessageHandler([&](const std::string &msg) {
        auto command = message_handler(msg);
        command_queue.enqueue(command);
    });
    WebsocketServer.start();

    auto TCP = TCPServer(9091);
    TCP.start();

    auto internal_comm_thread = std::thread([&] {
        auto last_msg_time = std::chrono::steady_clock::now();
        while (!stop){
            auto now = std::chrono::steady_clock::now();
            auto cmd = command_queue.dequeue();
            if (cmd.has_value()) {
                json j = json::parse(cmd.value());

                if (j["command"] == "stop" or j["command"] == "reset_camera") {
                    TCP.writeToAllClients(cmd.value());
                } else if (now - last_msg_time > std::chrono::milliseconds(200)) {
                    last_msg_time = now;
                    TCP.writeToAllClients(cmd.value());
                }
            }
        }
    });

    auto handler_proto = [&] (const std::string& message) {
        cv::Mat decoded_frame = decodeImageFromProto(message);
        Viewer->addFrame(decoded_frame);
    };

    auto udp_server = std::make_unique<UDPServer>(8080, handler_proto);

    udp_server->start();

    auto fps = 30;
    auto frame_interval = std::chrono::milliseconds(1000 / fps);

    std::cout << "Press a key in Video View to end..." << std::endl;
    while (true) {
        Viewer->display();
        if(cv::waitKey(frame_interval.count()) >= 0) break;
    }

    std::cout << "Stopping..." << std::endl;
    stop = true;

    WebsocketServer.stop();
    TCP.stop();
    udp_server->stop();

    std::cout << "Joining..." << std::endl;
    internal_comm_thread.join();
    std::cout << "Joined" << std::endl;

    return 0;
}