#ifndef REALTIMESYSROVER_UDP_SERVER2_HPP
#define REALTIMESYSROVER_UDP_SERVER2_HPP

#include <iostream>
#include <boost/asio.hpp>
#include <functional>

using udp = boost::asio::ip::udp;
namespace asio = boost::asio;

class UDPServer {
public:
    UDPServer(int port, std::function<void(const std::string&)> handler);
    void start();
    ~UDPServer();

private:
    std::function<void(const std::string&)> _messageHandler;
    std::tuple<std::string, size_t, udp::endpoint> receiveMessage();
    int receiveMessageSize();
    void sendMessage(const std::string& message, const boost::asio::ip::udp::endpoint& remote_endpoint);
    void standardResponse(const udp::endpoint& remote_endpoint);

    int _port;
    asio::io_context _io_context;
    udp::socket _socket;
    std::thread _thread;
    std::mutex _m;
};

#endif //REALTIMESYSROVER_UDP_SERVER2_HPP