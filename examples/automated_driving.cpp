// This file can be tested together with "RealtimeSystems\RealtimeSysRover\UDP\udp_client_json_dummy.py"

#include <iostream>
#include <string>
#include <vector>

#include "udp_server/udp_server.hpp"
#include "video_viewer/video_viewer.hpp"
#include "autonomous_driving/autonomous_driving.hpp"
#include "nlohmann/json.hpp"
#include "object_detection/object_detection.hpp"
#include "my_messages.pb.h"
#include "safe_queue/safe_queue.hpp"
#include "tcp_server/tcp_server_lib.hpp"
#include "helpers/gui_helper.hpp"

cv::Mat decodeImageFromProto (const std::string& frame) {
    VideoFeed video_feed;
    video_feed.ParseFromString(frame);

    std::vector<uchar> encoded_frame(video_feed.messagefeed().begin(), video_feed.messagefeed().end());
    cv::Mat decoded_frame = cv::imdecode(encoded_frame, cv::IMREAD_COLOR);
    return decoded_frame;
}

int main() {
    /*
    AutonomousDriving autonomousDriving;
    int x = 0;
    int y = -60;
    auto coordinates = autonomousDriving.formatCommand(x, y);
    std::cout << coordinates << std::endl;
    */
    SafeQueue<std::string> command_queue;
    std::atomic<bool> stop{false};

    auto TCP = TCPServer(9091);
    TCP.start();

    auto internal_comm_thread = std::thread([&] {
        auto last_msg_time = std::chrono::steady_clock::now();
        while (!stop){
            auto now = std::chrono::steady_clock::now();
            auto cmd = command_queue.dequeue();
            if (cmd.has_value()) {
                json j = json::parse(cmd.value());

                if (j["command"] == "stop") {
                    last_msg_time = now;
                    TCP.writeToAllClients(cmd.value());
                } else if (now - last_msg_time > std::chrono::milliseconds(100)) {
                    last_msg_time = now;
                    TCP.writeToAllClients(cmd.value());
                }
            }
        }
    });

    auto AutonomousDriver = std::make_unique<AutonomousDriving>();
    auto Viewer = std::make_unique<VideoViewer>();
    auto ObjectDetector = std::make_unique<ObjectDetection>();

    ObjectDetector->run();
    AutonomousDriver->run();

    auto handler_proto = [&] (const std::string& message) {
        cv::Mat decoded_frame = decodeImageFromProto(message);
        if(ObjectDetector->_running)
            ObjectDetector->addLatestFrame (decoded_frame);

        auto detection = ObjectDetector->getLatestDetection();

        AutonomousDriver->addLatestDetection(detection);
        auto command = AutonomousDriver->getLatestCommand();
        if (command.has_value()) {
            // std::cout << command.value() << std::endl;
            auto json_command = message_handler(command.value());
            command_queue.enqueue(json_command);
        }

        if (detection.has_value()) {
            decoded_frame = ObjectDetector->drawDetections(decoded_frame, detection);
        }
        Viewer->addFrame(decoded_frame);
    };

    auto udp_server = std::make_unique<UDPServer>(8080, handler_proto);

    udp_server->start();

    auto fps = 30;
    auto frame_interval = std::chrono::milliseconds(1000 / fps);
    while (true) {
        Viewer->display();
        if(cv::waitKey(frame_interval.count()) >= 0) break;
    }
    std::cout << "Stopping camera feed" << std::endl;
    ObjectDetector->stop();
    stop = true;
    TCP.stop();
}