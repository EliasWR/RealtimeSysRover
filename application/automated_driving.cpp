#include <chrono>
#include <iostream>
#include <string>

#include "nlohmann/json.hpp"

#include "autonomous_driving/autonomous_driving.hpp"
#include "helpers/gui_helper.hpp"
#include "helpers/network_helper.hpp"
#include "object_detection/object_detection.hpp"
#include "safe_queue/safe_queue.hpp"
#include "tcp_server/tcp_server_lib.hpp"
#include "udp_server/udp_server.hpp"
#include "video_viewer/video_viewer.hpp"

using json = nlohmann::json;

int main() {
    std::chrono::steady_clock::time_point last_command_time;
    std::chrono::steady_clock::time_point last_detection_time;
    std::optional<std::string> last_command;
    std::optional<Detection> last_detection;
    const std::chrono::milliseconds command_duration{50};
    const std::chrono::milliseconds detection_duration{0};

    SafeQueue<std::string> command_queue;
    std::atomic<bool> stop_comm_thread = false;

    auto TCP = TCP::TCPServer(9091);
    auto AutonomousDriver = AutonomousDriving();
    auto Viewer = VideoViewer();
    auto ObjectDetector = ObjectDetection();

    TCP.start();
    AutonomousDriver.run();
    ObjectDetector.run();

    auto internal_comm_thread = std::thread([&] {
        auto last_msg_time = std::chrono::steady_clock::now();

        while (!stop_comm_thread) {
            auto now = std::chrono::steady_clock::now();
            auto cmd = command_queue.dequeue();

            if (cmd.has_value()) {
                json j = json::parse(cmd.value());

                if (j["command"] == "stop") {
                    TCP.writeToAllClients(cmd.value());
                } else if (now - last_msg_time > std::chrono::milliseconds(100)) {
                    last_msg_time = now;
                    TCP.writeToAllClients(cmd.value());
                }
            }
        }
    });

    auto handler_proto = [&](const std::string &message) {
        auto now = std::chrono::steady_clock::now();
        auto decoded_frame = decodeImageFromProto(message);
        if (ObjectDetector.running) {
            ObjectDetector.addLatestFrame(decoded_frame);
        }

        auto detection = ObjectDetector.getLatestDetection();

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

        AutonomousDriver.addLatestDetection(detection);
        auto command = AutonomousDriver.getLatestCommand();

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
            auto json_command = GUI::message_handler(command.value());
            command_queue.enqueue(json_command);
        }

        if (detection != std::nullopt) {
            ObjectDetector.drawDetections(decoded_frame, detection);
        }
        Viewer.addFrame(decoded_frame);
    };

    auto UDP = UDPServer(8080, handler_proto);
    UDP.start();

    auto fps = 30;
    auto frame_interval = std::chrono::milliseconds(1000 / fps);
    while (true) {
        Viewer.display();
        if (cv::waitKey(frame_interval.count()) >= 0) break;
    }

    std::cout << "Stopping program" << std::endl;
    stop_comm_thread = true;
    TCP.stop();
    std::cout << "TCP stopped" << std::endl;
    AutonomousDriver.stop();
    std::cout << "AutonomousDriver stopped" << std::endl;
    ObjectDetector.stop();
    std::cout << "ObjectDetector stopped" << std::endl;
    UDP.stop();
    std::cout << "UDP stopped" << std::endl;
    internal_comm_thread.join();
    std::cout << "internal_comm_thread stopped" << std::endl;
    return 0;
}