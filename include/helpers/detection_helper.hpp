#ifndef REALTIMESYSROVER_Detection_HPP
#define REALTIMESYSROVER_Detection_HPP

#include <opencv2/opencv.hpp>
#include <vector>

struct Detection {
    std::vector<cv::Rect> boxes;
    std::vector<float> confidences;
    std::vector<int> classIds;
};

#endif //REALTIMESYSROVER_Detection_HPP