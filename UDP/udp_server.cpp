#include <iostream>
#include <boost/asio.hpp>
#include <opencv2/opencv.hpp>
#include "my_messages.pb.h"

using boU = boost::asio::ip::udp;
namespace boA = boost::asio;

const int port = 8080;

int main() {
    boA::io_context io_context;
    boU::socket mySocket(io_context, boU::endpoint(boU::v4(), port));

    std::cout << "Server is listening on port " << port << std::endl;
    cv::namedWindow("VideoFeed", cv::WINDOW_AUTOSIZE); // Create a window for display.

    while (true) {
        // Receive message from client
        boU::endpoint remote_endpoint;
        std::vector<char> recv_buffer(65507); // Increase buffer size for maximum UDP packet size
        size_t len = mySocket.receive_from(boA::buffer(recv_buffer), remote_endpoint);

        // Deserialize message using protobuf
        VideoFeed video_feed;
        video_feed.ParseFromArray(recv_buffer.data(), len);

        // Assuming the received data is JPEG encoded, we need to decode it.
        std::vector<uchar> encoded_frame(video_feed.messagefeed().begin(), video_feed.messagefeed().end());
        cv::Mat decoded_frame = cv::imdecode(encoded_frame, cv::IMREAD_COLOR); // Decode JPEG data

        // Check if the frame is empty
        if (decoded_frame.empty()) {
            std::cerr << "Decoded frame is empty or cannot be decoded." << std::endl;
            continue;
        }

        // Display the frame
        cv::imshow("VideoFeed", decoded_frame);

        // Check for 'q' key to quit
        if (cv::waitKey(1) == 'q') {
            break;
        }

        // Process video frame and determine instruction
        Instruction instruction;
        instruction.set_messageinstruction("This is a response from the server.");

        // Serialize instruction message
        std::string serialized_instruction;
        instruction.SerializeToString(&serialized_instruction);

        // Send instruction back to client
        mySocket.send_to(boA::buffer(serialized_instruction), remote_endpoint);
    }

    cv::destroyWindow("VideoFeed"); // Clean up the window

    return 0;
}
