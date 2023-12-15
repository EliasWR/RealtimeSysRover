#include <iostream>
#include <string>

#include "helpers/network_helper.hpp"
#include "object_detection/object_detection.hpp"
#include "udp_server/udp_server.hpp"
#include "video_viewer/video_viewer.hpp"

int main() {
  auto Viewer = VideoViewer();
  auto ObjectDetector = ObjectDetection();
  ObjectDetector.run();

  auto handler_json = [&](const std::string &message) {
    cv::Mat decoded_frame = decodeImageFromJson(message);
    Viewer.addFrame(decoded_frame);
  };

  auto handler_proto = [&](const std::string &message) {
    cv::Mat decoded_frame = decodeImageFromProto(message);
    if (ObjectDetector.running)
      ObjectDetector.addLatestFrame(decoded_frame);

    auto detection = ObjectDetector.getLatestDetection();

    if (detection.has_value()) {
      decoded_frame = ObjectDetector.drawDetections(decoded_frame, detection);
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
  std::cout << "Stopping camera feed" << std::endl;
  ObjectDetector.stop();
  UDP.stop();
}