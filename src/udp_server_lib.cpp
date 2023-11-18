#include "udp_server/udp_server.hpp"

using udp = boost::asio::ip::udp;
namespace asio = boost::asio;

UDPServer::UDPServer(int port, std::unique_ptr<MessageHandler> handler)
        : _port(port), _socket(_io_context, udp::endpoint(udp::v4(), _port)), _messageHandler(std::move(handler)) {}

void UDPServer::start() {
    std::cout << "Server is listening on port " << _port << std::endl;

    _thread = std::thread([&] {
        _messageHandler->openWindow();
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
}

std::tuple<std::vector<char>, size_t, udp::endpoint> UDPServer::receiveMessage() {
    udp::endpoint remote_endpoint;

    std::vector<char> recv_buffer(65507);
    size_t len = _socket.receive_from(asio::buffer(recv_buffer), remote_endpoint);
    // std::cout << "Message received." << std::endl;
    // std::vector<char> message(recv_buffer.begin(), recv_buffer.begin() + len);
    return std::make_tuple(recv_buffer, len, remote_endpoint);
}

void UDPServer::standardResponse(const udp::endpoint& remote_endpoint) {
    Instruction instruction;
    instruction.set_messageinstruction("Message received.");
    std::string serialized_instruction;
    instruction.SerializeToString(&serialized_instruction);
    sendMessage(serialized_instruction, remote_endpoint);
    // std::cout << "Response sent." << std::endl;
}

void UDPServer::sendMessage(const std::string& message, const udp::endpoint& remote_endpoint) {
    _socket.send_to(asio::buffer(message), remote_endpoint);
}

UDPServer::~UDPServer() {
    _socket.close();
    _io_context.stop();
    _thread.join();
}