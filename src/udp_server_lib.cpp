#include "udp_server/udp_server.hpp"
#include "my_messages.pb.h"
#include "udp_server/video_feed_handler.hpp"

using udp = boost::asio::ip::udp;
namespace asio = boost::asio;

template class UDPServer<VideoFeedHandler>;

template<class T>
UDPServer<T>::UDPServer(int port, std::unique_ptr<T> handler)
        : _port(port), _socket(_io_context, udp::endpoint(udp::v4(), _port)), _messageHandler(std::move(handler)) {}

template<class T>
void UDPServer<T>::start() {
    std::cout << "Server is listening on port " << _port << std::endl;

    _thread = std::jthread([&] {
        try {
            while (true) {
                auto [message, endpoint] = receiveMessage();
                _messageHandler->handleMessage(message);
                standardResponse(endpoint);
            }
        } catch (const std::exception& e) {
            std::cerr << "Exception caught in thread: " << e.what() << '\n';
        } catch (...) {
            std::cerr << "Unknown exception caught in thread.\n";
        }
    });
    auto _ = _thread.get_id(); // Making Thread not appear gray
}

template<class T>
std::pair<std::string, udp::endpoint> UDPServer<T>::receiveMessage() {
    udp::endpoint remote_endpoint;
    std::vector<char> recv_buffer(65507);
    size_t len = _socket.receive_from(asio::buffer(recv_buffer), remote_endpoint);

    std::string message(recv_buffer.begin(), recv_buffer.begin() + len);
    return std::make_pair(message, remote_endpoint);
}

template<class T>
void UDPServer<T>::standardResponse(const udp::endpoint& remote_endpoint) {
    Instruction instruction;
    instruction.set_messageinstruction("Message received.");
    std::string serialized_instruction;
    instruction.SerializeToString(&serialized_instruction);
    sendMessage(serialized_instruction, remote_endpoint);
}

template<class T>
void UDPServer<T>::sendMessage(const std::string& message, const udp::endpoint& remote_endpoint) {
    _socket.send_to(asio::buffer(message), remote_endpoint);
}

template<class T>
UDPServer<T>::~UDPServer() {
    _socket.close();
    _io_context.stop();
}