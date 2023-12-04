#ifndef REALTIMESYSROVER_WS_SERVER_LIB_HPP
#define REALTIMESYSROVER_WS_SERVER_LIB_HPP

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/beast/websocket.hpp>
#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <atomic>

namespace beast = boost::beast;
namespace asio = boost::asio;
namespace websocket = beast::websocket;

using tcp = boost::asio::ip::tcp;

class WSConnection {
public:
  explicit WSConnection(tcp::socket socket);
  void start();
  std::string receiveMessage();
  void setMessageHandler(std::function<void(const std::string&)> handler);

private:
  websocket::stream<tcp::socket> _socket;
  std::function<void(const std::string&)> _message_handler;
};

class WSServer {
public:
  explicit WSServer(unsigned short port);
  void start();
  void stop();
  void setMessageHandler(std::function<void(const std::string&)> handler);

private:
  asio::io_context _ioc;
  tcp::acceptor _acceptor;
  std::thread _acceptor_thread;
  std::function<void(const std::string&)> _message_handler;
  std::mutex _m;
  std::atomic<bool> _is_running{false};
};



#endif//REALTIMESYSROVER_WS_SERVER_LIB_HPP
