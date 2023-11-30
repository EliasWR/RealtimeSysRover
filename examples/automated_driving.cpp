// This file can be tested together with "RealtimeSystems\RealtimeSysRover\UDP\udp_client_json_dummy.py"

#include <iostream>
#include <string>
#include <vector>

#include "udp_server/udp_server.hpp"
#include "video_viewer/video_viewer.hpp"
#include "nlohmann/json.hpp"
#include "object_detection/object_detection.hpp"
#include "my_messages.pb.h"


cv::Mat decodeImageFromProto (const std::string& frame) {
    VideoFeed video_feed;
    video_feed.ParseFromString(frame);

    std::vector<uchar> encoded_frame(video_feed.messagefeed().begin(), video_feed.messagefeed().end());
    cv::Mat decoded_frame = cv::imdecode(encoded_frame, cv::IMREAD_COLOR);
    return decoded_frame;
}

int main() {
    auto Viewer = std::make_unique<VideoViewer>();
    auto ObjectDetector = std::make_unique<ObjectDetection>();
    ObjectDetector->run();

    auto handler_proto = [&] (const std::string& message) {
        cv::Mat decoded_frame = decodeImageFromProto(message);
        if(ObjectDetector->_running)
            ObjectDetector->addLatestFrame (decoded_frame);

        auto detection = ObjectDetector->getLatestDetection();

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
}