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
  using Callback = std::function<void(const std::string &request, std::string &response)>;

  explicit WSConnection(tcp::socket socket);
  void start();
  void set_callback(Callback callback);
  int receiveMessageSize();
  virtual void on_message(beast::flat_buffer &buffer);

private:
  websocket::stream<tcp::socket> _socket;
  Callback _callback;
};

class WSServer {
public:
  explicit WSServer(unsigned short port);
  void set_callback(WSConnection::Callback callback);
  void start();
  void stop();

private:
  asio::io_context _ioc;
  tcp::acceptor _acceptor;
  std::thread _acceptor_thread;
  WSConnection::Callback _callback;
  std::mutex _m;
  std::atomic<bool> _is_running{false};
};



#endif//REALTIMESYSROVER_WS_SERVER_LIB_HPP
