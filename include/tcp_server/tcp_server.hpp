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
  explicit Connection(std::unique_ptr<tcp::socket> socket);
  virtual void run();
  std::string getIPv4();
  //using Callback = std::function<void(Connection& conn, const std::string& request, std::string& response)>;
  using Callback = std::function<void(const std::string& request, std::string& response)>;

  void setCallback(Callback& callback);
  int receiveMessageSize();
  std::string receiveMessage();
  void writeMsg(const std::string& msg);
  std::unique_ptr<tcp::socket>& getSocket() { return _socket; }

protected:
  std::unique_ptr<tcp::socket> _socket;
  Callback _callback;
  std::jthread _thread;

};



class TCPServer_ {
public:
  explicit TCPServer_(unsigned short port);
  void start();
  void stop();
  bool is_running() const;

  virtual void set_callback(Connection::Callback callback);

protected:
  virtual void accept();

  std::jthread _ioc_thread;
  asio::io_context _io_context;
  tcp::acceptor _acceptor;
  std::jthread _acceptor_thread;
  std::unique_ptr<boost::asio::thread_pool> _thread_pool;
  Connection::Callback _callback;

  unsigned short _port;
  std::atomic<bool> _is_running{false};
};



#endif//REALTIMESYSROVER__TCP_SERVER2_HPP
