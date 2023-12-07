#ifndef REALTIMESYSROVER_Detection_HPP
#define REALTIMESYSROVER_Detection_HPP

#include <opencv2/opencv.hpp>
#include <vector>
#include <thread>

struct Detection {
    std::vector<cv::Rect> boxes;
    std::vector<float> confidences;
    std::vector<int> classIds;
    std::pair<int, int> frameSize;
};

#endif //REALTIMESYSROVER_Detection_HPP