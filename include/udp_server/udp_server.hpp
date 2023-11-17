
#ifndef REALTIMESYSROVER_UDP_SERVER_HPP
#define REALTIMESYSROVER_UDP_SERVER_HPP

#include <iostream>
#include <boost/asio.hpp>
#include <opencv2/opencv.hpp>
#include "message_handling/message_handler.hpp"
#include "udp_server/video_feed_handler.hpp"
#include "my_messages.pb.h"

using udp = boost::asio::ip::udp;
namespace asio = boost::asio;

class UDPServer {
public:
    UDPServer(int port);
    void start();
    ~UDPServer();


private:
    asio::io_context _io_context;
    udp::socket _socket;
    int _port;
    std::thread _thread;
    MessageHandler _messageHandler;
};

#endif //REALTIMESYSROVER_UDP_SERVER_HPP

/*
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
    std::tuple<std::vector<char>, size_t, udp::endpoint> receiveMessage();
    void sendMessage(const std::string& message, const boost::asio::ip::udp::endpoint& remote_endpoint);
    void standardResponse(const udp::endpoint& remote_endpoint);

    asio::io_context _io_context;
    udp::socket _socket;
    int _port;
    std::jthread _thread;
};

template class UDPServer<VideoFeedHandler>;

#endif //REALTIMESYSROVER_UDP_SERVER2_HPP

*/