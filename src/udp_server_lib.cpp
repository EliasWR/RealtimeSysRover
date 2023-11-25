#include "udp_server/udp_server.hpp"

using udp = boost::asio::ip::udp;
namespace asio = boost::asio;

UDPServer::UDPServer(int port, std::function<void(const std::string&)> handler)
        : _port(port), _socket(_io_context, udp::endpoint(udp::v4(), _port)), _messageHandler(std::move(handler)) {}

void UDPServer::start() {
    std::cout << "Server is listening on port " << _port << std::endl;

    _thread = std::thread([&] {
        try {
            while (true) {
                auto [message, len, endpoint] = receiveMessage();
                _messageHandler(message);
                //standardResponse(endpoint);
            }
        } catch (const std::exception& e) {
            std::cerr << "Exception caught in thread: " << e.what() << '\n';
        } catch (...) {
            std::cerr << "Unknown exception caught in thread.\n";
        }
    });
}

std::tuple<std::string, size_t, udp::endpoint> UDPServer::receiveMessage() {
    udp::endpoint remote_endpoint;

    std::vector<char> recv_buffer(65507);
    size_t len = _socket.receive_from(asio::buffer(recv_buffer), remote_endpoint);
    std::cout << "Message length: " << len << std::endl;
    std::string recv_message(recv_buffer.begin(), recv_buffer.begin() + len);
    // std::cout << "Message received." << std::endl;
    // std::vector<char> message(recv_buffer.begin(), recv_buffer.begin() + len);
    return std::make_tuple(recv_message, len, remote_endpoint);
}

int UDPServer::receiveMessageSize() {
    std::vector<char> recv_buffer(4);
    size_t len = _socket.receive(asio::buffer(recv_buffer));
    std::string data(recv_buffer.begin(), recv_buffer.begin() + len);
    try {
        return std::stoi(data);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
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


