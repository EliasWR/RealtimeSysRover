#ifndef REALTIMESYSROVER_UDP_SERVER2_HPP
#define REALTIMESYSROVER_UDP_SERVER2_HPP

#include <iostream>
#include <boost/asio.hpp>
#include <opencv2/opencv.hpp>
#include "message_handling/message_handler.hpp"
#include "udp_server/video_feed_handler.hpp"

using udp = boost::asio::ip::udp;
namespace asio = boost::asio;

template<class T>
class UDPServer {
public:
    UDPServer(int port, std::unique_ptr<MessageHandler> handler);
    void start();
    ~UDPServer();

private:
    std::unique_ptr<MessageHandler> _messageHandler;
    std::pair<std::vector<char>, udp::endpoint> receiveMessage();
    void sendMessage(const std::string& message, const boost::asio::ip::udp::endpoint& remote_endpoint);
    void standardResponse(const udp::endpoint& remote_endpoint);

    boost::asio::io_context _io_context;
    boost::asio::ip::udp::socket _socket;
    int _port;
    std::jthread _thread;
};

template class UDPServer<VideoFeedHandler>;

#endif //REALTIMESYSROVER_UDP_SERVER2_HPP