// This file can be tested together with python files in "RealtimeSystems\realtime_sys_rover\udp\"

#include <iostream>
#include <string>
#include <vector>

#include "nlohmann/json.hpp"

#include "helpers/network_helper.hpp"
#include "object_detection/object_detection.hpp"
#include "udp_server/udp_server.hpp"
#include "video_viewer/video_viewer.hpp"

int main() {
  auto Viewer = VideoViewer();
  auto ObjectDetector = ObjectDetection();

  auto handler_json = [&](const std::string &message) {
    cv::Mat decoded_frame = decodeImageFromJson(message);
    Viewer.addFrame(decoded_frame);
  };

  auto handler_proto = [&](const std::string &message) {
    cv::Mat decoded_frame = decodeImageFromProto(message);
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
  std::cout << "Stopping camera feed" << std::endl;
  UDP.stop();
}