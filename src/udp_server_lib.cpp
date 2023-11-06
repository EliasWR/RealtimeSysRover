#include "udp_server/udp_server.hpp"
#include "my_messages.pb.h"

using udp = boost::asio::ip::udp;
namespace asio = boost::asio;

UDPServer::UDPServer(int port)
        : _port(port), _socket(_io_context, udp::endpoint(udp::v4(), _port)) {}

void UDPServer::start() {
    std::cout << "Server is listening on port " << _port << std::endl;
    cv::namedWindow("VideoFeed", cv::WINDOW_AUTOSIZE);

    _thread = std::jthread([&] {
        try {
            while (true) {
                receiveMessage();
                if (cv::waitKey(1) == 'q') {
                    break;
                }
            }
        } catch (const std::exception& e) {
            std::cerr << "Exception caught in thread: " << e.what() << '\n';
        } catch (...) {
            std::cerr << "Unknown exception caught in thread.\n";
        }
    });
    auto _ = _thread.get_id(); // Make thread not appear unused

    cv::destroyWindow("VideoFeed");
}

void UDPServer::receiveMessage() {
    udp::endpoint remote_endpoint;
    std::vector<char> recv_buffer(65507);
    size_t len = _socket.receive_from(asio::buffer(recv_buffer), remote_endpoint);

    VideoFeed video_feed;
    video_feed.ParseFromArray(recv_buffer.data(), len);

    std::vector<uchar> encoded_frame(video_feed.messagefeed().begin(), video_feed.messagefeed().end());
    cv::Mat decoded_frame = cv::imdecode(encoded_frame, cv::IMREAD_COLOR);

    if (!decoded_frame.empty()) {
        displayFrame(decoded_frame);
    } else {
        std::cerr << "Decoded frame is empty or cannot be decoded." << std::endl;
    }

    Instruction instruction;
    instruction.set_messageinstruction("This is a response from the server.");
    std::string serialized_instruction;
    instruction.SerializeToString(&serialized_instruction);

    sendMessage(serialized_instruction, remote_endpoint);
}

void UDPServer::displayFrame(const cv::Mat& frame) {
    cv::imshow("VideoFeed", frame);
}

void UDPServer::sendMessage(const std::string& message, const udp::endpoint& remote_endpoint) {
    _socket.send_to(asio::buffer(message), remote_endpoint);
}

UDPServer::~UDPServer() {
    _socket.close();
    _io_context.stop();
}

/*
int main() {
    asio::io_context io_context;
    udp::socket mySocket(io_context, udp::endpoint(udp::v4(), port));

    std::cout << "Server is listening on port " << port << std::endl;
    cv::namedWindow("VideoFeed", cv::WINDOW_AUTOSIZE); // Create a window for display.

    while (true) {
        // Receive message from client
        udp::endpoint remote_endpoint;
        std::vector<char> recv_buffer(65507); // Increase buffer size for maximum UDP packet size
        size_t len = mySocket.receive_from(asio::buffer(recv_buffer), remote_endpoint);

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
        mySocket.send_to(asio::buffer(serialized_instruction), remote_endpoint);
    }

    cv::destroyWindow("VideoFeed"); // Clean up the window

    return 0;
}
*/