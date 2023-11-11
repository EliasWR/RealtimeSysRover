#ifndef REALTIMESYSROVER_UDP_SERVER2_HPP
#define REALTIMESYSROVER_UDP_SERVER2_HPP

#include <iostream>
#include <boost/asio.hpp>
#include <opencv2/opencv.hpp>

using udp = boost::asio::ip::udp;
namespace asio = boost::asio;

class UDPServer {
public:
    UDPServer(int port);
    void start();
    ~UDPServer();

private:
    std::string receiveMessage();
    void sendMessage(const std::string& message, const boost::asio::ip::udp::endpoint& remote_endpoint);
    void standardResponse (const udp::endpoint& remote_endpoint);

    boost::asio::io_context _io_context;
    boost::asio::ip::udp::socket _socket;
    int _port;
    std::jthread _thread;
};

#endif //REALTIMESYSROVER_UDP_SERVER2_HPP