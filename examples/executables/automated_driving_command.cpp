// This file can be tested together with "RealtimeSystems\realtime_sys_rover\udp\""

#include <iostream>
#include <string>

#include "nlohmann/json.hpp"

#include "autonomous_driving/autonomous_driving.hpp"
#include "helpers/network_helper.hpp"
#include "object_detection/object_detection.hpp"
#include "udp_server/udp_server.hpp"
#include "video_viewer/video_viewer.hpp"

int main() {
  auto AutonomousDriver = AutonomousDriving();
  auto Viewer = VideoViewer();
  auto ObjectDetector = ObjectDetection();

  ObjectDetector.run();
  AutonomousDriver.run();

  auto handler_proto = [&](const std::string &message) {
    cv::Mat decoded_frame = decodeImageFromProto(message);
    if (ObjectDetector.running)
      ObjectDetector.addLatestFrame(decoded_frame);

    auto detection = ObjectDetector.getLatestDetection();

    AutonomousDriver.addLatestDetection(detection);
    auto command = AutonomousDriver.getLatestCommand();
    if (command.has_value()) {
      std::cout << command.value() << std::endl;
    }

    if (detection != std::nullopt) {
      decoded_frame = ObjectDetector.drawDetections(decoded_frame, detection);
      std::cout << "Added detection to frame" << std::endl;
    }
    Viewer.addFrame(decoded_frame);
  };

  auto UDP = UDPServer(8080, handler_proto);
  UDP.start();

  int fps = 30;
  auto frame_interval = std::chrono::milliseconds(1000 / fps);
  while (true) {
    Viewer.display();
    if (cv::waitKey(static_cast<int>(frame_interval.count())) >= 0) break;
  }
  std::cout << "Stopping camera feed" << std::endl;
  ObjectDetector.stop();
  UDP.stop();
}