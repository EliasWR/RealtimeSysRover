// This file can be tested together with "RealtimeSystems\RealtimeSysRover\udp\udp_client_json_dummy.py"

#include <iostream>
#include <string>
#include <vector>
#include <chrono>

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
    std::chrono::steady_clock::time_point last_command_time;
    std::chrono::steady_clock::time_point last_detection_time;
    std::optional<std::string> last_command;
    std::optional<Detection> last_detection;
    const std::chrono::milliseconds command_duration{50};
    const std::chrono::milliseconds detection_duration{0};
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
        auto now = std::chrono::steady_clock::now();
        cv::Mat decoded_frame = decodeImageFromProto(message);
        if(ObjectDetector->_running)
            ObjectDetector->addLatestFrame (decoded_frame);

        auto detection = ObjectDetector->getLatestDetection();

        // Only use last detection if it is not too old
        if (detection.has_value()) {
            last_detection_time = now;
            last_detection = detection;
        } else if (last_detection.has_value() && now - last_detection_time < detection_duration) {
            detection = last_detection;
        } else {
            std::cout << "No available detection" << std::endl;
            last_detection.reset();
        }

        AutonomousDriver->addLatestDetection(detection);
        auto command = AutonomousDriver->getLatestCommand();

        // Only use last command if it is not too old
        if (command.has_value()) {
            last_command_time = now;
            last_command = command;
        } else if (last_command.has_value() && now - last_command_time < command_duration) {
            command = last_command;
        } else {
            std::cout << "No auto command" << std::endl;
            last_command.reset();
        }

        if (command.has_value()) {
            // std::cout << command.value() << std::endl;
            auto json_command = message_handler(command.value());
            command_queue.enqueue(json_command);


            /*json json_command_json = json::parse(json_command);
            std::string overlay_text = "No detection";
            try{
                int left_velocity = json_command_json["left_velocity"];
                int right_velocity = json_command_json["right_velocity"];
                overlay_text = "Left: " + std::to_string(left_velocity) + " Right: " + std::to_string(right_velocity);
            } catch (std::exception& e) {
                std::cout << "Could not make overlay" << std::endl;
            }
            cv::putText(decoded_frame, overlay_text, cv::Point(10, 50), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 0, 255), 2);*/
        }

        if (detection != std::nullopt) {
            ObjectDetector->drawDetections(decoded_frame, detection);
            std::cout << "Added detection to frame" << std::endl;
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