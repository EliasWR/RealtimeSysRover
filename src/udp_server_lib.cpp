#include "udp_server/udp_server.hpp"

UDPServer::UDPServer(int port) : _port(port), _socket(_io_context, udp::endpoint(udp::v4(), _port)) {}

void UDPServer::start() {
    std::cout << "Server is listening on port " << _port << std::endl;
    _thread = std::thread ([&]{
        try {
            while (true) {
                // TODO Implement thread here
            }
        } catch (const std::exception& e) {
            std::cerr << "Exception caught in thread: " << e.what() << '\n';
        } catch (...) {
            std::cerr << "Unknown exception caught in thread.\n";
        }
    });
}

UDPServer::~UDPServer() {
    _socket.close();
    _io_context.stop();
    _thread.join();
}
/*


#include "udp_server/udp_server.hpp"
#include "my_messages.pb.h"

using udp = boost::asio::ip::udp;
namespace asio = boost::asio;

template <class T>
UDPServer<T>::UDPServer(int port, std::unique_ptr<MessageHandler> handler)
        : _port(port), _socket(_io_context, udp::endpoint(udp::v4(), _port)), _messageHandler(std::move(handler)) {}

template<class T>
void UDPServer<T>::start() {
    std::cout << "Server is listening on port " << _port << std::endl;

    _thread = std::jthread([&] {
        try {
            while (true) {
                auto [message, len, endpoint] = receiveMessage();
                _messageHandler->handleMessage(message, len);
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
std::tuple<std::vector<char>, size_t, udp::endpoint> UDPServer<T>::receiveMessage() {
    udp::endpoint remote_endpoint;

    std::vector<char> recv_buffer(65507);
    size_t len = _socket.receive_from(asio::buffer(recv_buffer), remote_endpoint);
    std::cout << "Message received." << std::endl;
    // std::vector<char> message(recv_buffer.begin(), recv_buffer.begin() + len);
    return std::make_tuple(recv_buffer, len, remote_endpoint);
}

template<class T>
void UDPServer<T>::standardResponse(const udp::endpoint& remote_endpoint) {
    Instruction instruction;
    instruction.set_messageinstruction("Message received.");
    std::string serialized_instruction;
    instruction.SerializeToString(&serialized_instruction);
    sendMessage(serialized_instruction, remote_endpoint);
    std::cout << "Response sent." << std::endl;
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
*/