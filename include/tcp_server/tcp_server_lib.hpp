#ifndef REALTIMESYSROVER_TCP_SERVER2_HPP
#define REALTIMESYSROVER_TCP_SERVER2_HPP

#include "helpers/network_helper.hpp"

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <utility>

namespace TCP {

  namespace beast = boost::beast;
  namespace asio = boost::asio;

  using tcp = boost::asio::ip::tcp;

  class Connection {
  public:
    explicit Connection(tcp::socket socket);
    void stop();
    void start();
    std::string getIPv4();

    void setMessageHandler(std::function<void(const std::string &)> handler);
    int receiveMessageSize();
    std::string receiveMessage();
    void writeMessage(const std::string &msg);
    void setDisconnectionHandler(const std::function<void(Connection *)> &handler);

  private:
    std::function<void(Connection *)> _disconnection_handler;
    std::function<void(const std::string &)> _message_handler;

    tcp::socket _socket;
    std::mutex _m;
    std::thread _thread;
    std::atomic<bool> _is_running {false};
  };

  class TCPServer {
  public:
    explicit TCPServer(unsigned short port);
    void start();
    void stop();

    void setMessageHandler(std::function<void(const std::string &)> handler);
    void writeToClient(size_t client_index, const std::string &msg);
    void writeToAllClients(const std::string &msg);
    void processTasks();

  private:
    std::queue<std::function<void()>> tasks;
    std::mutex _task_m;
    std::condition_variable _tasks_cond;
    std::thread _tasks_thread;
    std::atomic<bool> _stop_task_thread {false};

    std::vector<std::unique_ptr<Connection>> _clients;

    asio::io_context _io_context;
    tcp::acceptor _acceptor;
    std::thread _acceptor_thread;
    std::function<void(const std::string &)> _message_handler;
    void handleDisconnection(Connection *conn);

    unsigned short _port;
    std::mutex _m;
    std::atomic<bool> _is_running {false};
  };
}// namespace TCP

#endif//REALTIMESYSROVER__TCP_SERVER2_HPP
