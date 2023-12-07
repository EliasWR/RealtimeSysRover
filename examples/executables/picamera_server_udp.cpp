// This file can be tested together with python files in "RealtimeSystems\RealtimeSysRover\udp\"

#include <iostream>
#include <string>
#include <vector>

#include "my_messages.pb.h"
#include "nlohmann/json.hpp"
#include "object_detection/object_detection.hpp"
#include "udp_server/udp_server.hpp"
#include "video_viewer/video_viewer.hpp"

std::string base64_decode(const std::string &in) {
  std::string out;
  std::vector<int> T(256, -1);
  for (int i = 0; i < 64; i++) T["ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[i]] = i;

  int val = 0, valb = -8;
  for (uchar c : in) {
    if (T[c] == -1) break;
    val = (val << 6) + T[c];
    valb += 6;
    if (valb >= 0) {
      out.push_back(char((val >> valb) & 0xFF));
      valb -= 8;
    }
  }
  return out;
}

cv::Mat decodeImageFromJson(const std::string &jsonString) {
  // Parse the JSON
  auto json = nlohmann::json::parse(jsonString);
  std::string encodedImage = json["image"];

  // Base64 Decode
  std::string decodedImageData = base64_decode(encodedImage);

  // Convert to vector of bytes
  std::vector<uchar> data(decodedImageData.begin(), decodedImageData.end());

  // Decode image
  cv::Mat image = cv::imdecode(data, cv::IMREAD_COLOR);

  return image;
}

cv::Mat decodeImageFromProto(const std::string &frame) {
  VideoFeed video_feed;
  video_feed.ParseFromString(frame);

  std::vector<uchar> encoded_frame(video_feed.messagefeed().begin(), video_feed.messagefeed().end());
  cv::Mat decoded_frame = cv::imdecode(encoded_frame, cv::IMREAD_COLOR);
  return decoded_frame;
}

int main() {
  auto Viewer = std::make_unique<VideoViewer>();
  auto ObjectDetector = std::make_unique<ObjectDetection>();

  auto handler_json = [&](const std::string &message) {
    cv::Mat decoded_frame = decodeImageFromJson(message);
    Viewer->addFrame(decoded_frame);
  };

  auto handler_proto = [&](const std::string &message) {
    cv::Mat decoded_frame = decodeImageFromProto(message);
    // ObjectDetector->detectObjects(decoded_frame);

    Viewer->addFrame(decoded_frame);
  };

  auto udp_server = std::make_unique<UDPServer>(8080, handler_proto);

  udp_server->start();

  auto fps = 30;
  auto frame_interval = std::chrono::milliseconds(1000 / fps);
  while (true) {
    Viewer->display();
    if (cv::waitKey(frame_interval.count()) >= 0) break;
  }
  std::cout << "Stopping camera feed" << std::endl;
}