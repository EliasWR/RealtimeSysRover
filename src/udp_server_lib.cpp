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
}

std::string UDPServer::receiveMessage() {
    udp::endpoint remote_endpoint;
    std::vector<char> recv_buffer(65507);
    size_t len = _socket.receive_from(asio::buffer(recv_buffer), remote_endpoint);

    std::string message(recv_buffer.begin(), recv_buffer.begin() + len);

    // std::cout << "Received message: " << message << std::endl;
    // standardResponse(remote_endpoint);

    return message;
}

void UDPServer::standardResponse (const udp::endpoint& remote_endpoint){
    Instruction instruction;
    instruction.set_messageinstruction("Message received.");
    std::string serialized_instruction;
    instruction.SerializeToString(&serialized_instruction);
    sendMessage(serialized_instruction, remote_endpoint);
}

void UDPServer::sendMessage(const std::string& message, const udp::endpoint& remote_endpoint) {
    _socket.send_to(asio::buffer(message), remote_endpoint);
}

UDPServer::~UDPServer() {
    _socket.close();
    _io_context.stop();
}