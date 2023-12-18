#ifndef REALTIMESYSROVER_UDP_SERVER2_HPP
#define REALTIMESYSROVER_UDP_SERVER2_HPP

#include <boost/asio.hpp>
#include <functional>
#include <iostream>
#include <mutex>

using udp = boost::asio::ip::udp;
namespace asio = boost::asio;

class UDPServer {
public:
  UDPServer(int port, std::function<void(const std::string &)> handler);
  void start();
  void stop();
  [[maybe_unused]] void sendMessage(const std::string &message, const boost::asio::ip::udp::endpoint &remote_endpoint);
  std::tuple<std::string, udp::endpoint> receiveMessage();
  std::atomic<bool> running = false;

private:
  std::function<void(const std::string &)> _messageHandler;
  [[maybe_unused]] int receiveMessageSize();

  int _port;
  asio::io_context _io_context;
  udp::socket _socket;
  std::thread _thread;
  std::mutex _m;
};

#endif//REALTIMESYSROVER_UDP_SERVER2_HPP