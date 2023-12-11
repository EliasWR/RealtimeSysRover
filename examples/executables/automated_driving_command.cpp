// This file can be tested together with "RealtimeSystems\realtime_sys_rover\udp\""

#include <iostream>
#include <string>
#include <vector>

#include "autonomous_driving/autonomous_driving.hpp"
#include "my_messages.pb.h"
#include "nlohmann/json.hpp"
#include "object_detection/object_detection.hpp"
#include "udp_server/udp_server.hpp"
#include "video_viewer/video_viewer.hpp"

cv::Mat decodeImageFromProto(const std::string &frame) {
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
  auto AutonomousDriver = std::make_unique<AutonomousDriving>();
  auto Viewer = std::make_unique<VideoViewer>();
  auto ObjectDetector = std::make_unique<ObjectDetection>();

  ObjectDetector->run();
  AutonomousDriver->run();

  auto handler_proto = [&](const std::string &message) {
    cv::Mat decoded_frame = decodeImageFromProto(message);
    if (ObjectDetector->_running)
      ObjectDetector->addLatestFrame(decoded_frame);

    auto detection = ObjectDetector->getLatestDetection();

    AutonomousDriver->addLatestDetection(detection);
    auto command = AutonomousDriver->getLatestCommand();
    if (command.has_value()) {
      std::cout << command.value() << std::endl;
    }

    if (detection != std::nullopt) {
      decoded_frame = ObjectDetector->drawDetections(decoded_frame, detection);
      std::cout << "Added detection to frame" << std::endl;
    }
    Viewer->addFrame(decoded_frame);
  };

  auto udp_server = std::make_unique<UDPServer>(8080, handler_proto);

  udp_server->start();

  int fps = 30;
  auto frame_interval = std::chrono::milliseconds(1000 / fps);
  while (true) {
    Viewer->display();
    if (cv::waitKey(static_cast<int>(frame_interval.count())) >= 0) break;
  }
  std::cout << "Stopping camera feed" << std::endl;
  ObjectDetector->stop();
}