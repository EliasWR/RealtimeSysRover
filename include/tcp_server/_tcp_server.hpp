#ifndef REALTIMESYSROVER_TCP_SERVER2_HPP
#define REALTIMESYSROVER_TCP_SERVER2_HPP

#include "network_helper.hpp"

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <string>
#include <functional>

namespace beast = boost::beast;
namespace asio = boost::asio;

using tcp = boost::asio::ip::tcp;

class Connection {
public:
  explicit Connection(tcp::socket socket);
  void start();

  using Callback = std::function<void(const std::string& request, std::string& response)>;

  void setCallback(Callback& callback);

private:
  tcp::socket socket_;
  Callback _callback;

  int _recieveSize();
};

class TCPServer {
public:
  TCPServer(unsigned short port, std::size_t thread_pool_size);
  void start();
  void stop();
  bool is_running() const;

  void set_callback(Connection::Callback callback);

private:
  void accept();

  unsigned short _port;
  std::jthread _ioc_thread;
  asio::io_context _io_context;
  tcp::acceptor _acceptor;
  Connection::Callback _callback;
  std::unique_ptr<asio::thread_pool> _thread_pool;
  std::atomic<bool> _is_running;
  std::unique_ptr<std::thread> _acceptor_thread;
};





#endif//REALTIMESYSROVER__TCP_SERVER2_HPP
