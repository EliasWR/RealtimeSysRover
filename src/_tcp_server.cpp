#include "tcp_server/_tcp_server.hpp"
#include <iostream>
#include <utility>

// Connection Implementation
Connection::Connection(tcp::socket socket) : socket_(std::move(socket)) {}

void Connection::setCallback(Callback& callback) {
  _callback = callback;
}

void Connection::start() {


  std::vector<char> data(8192);
  boost::system::error_code error;
  size_t length = socket_.read_some(asio::buffer(data), error);

  if (!error) {
    std::string request(data.begin(), data.begin() + length);
    std::string response;

    if (_callback) {
      _callback(request, response);
    }

    asio::write(socket_, asio::buffer(response), error);
  }
  socket_.close();
}

int Connection::_recieveSize() {
  std::array<unsigned char, 4> buf{};
  boost::asio::read(socket_, boost::asio::buffer(buf), boost::asio::transfer_exactly(4));
  return bytes_to_int(buf);
}

// TCPServer Implementation
TCPServer::TCPServer(unsigned short port, std::size_t thread_pool_size)
    : _port(port),
      _acceptor(_io_context, tcp::endpoint(tcp::v4(), port)),
      _thread_pool(std::make_unique<boost::asio::thread_pool>(thread_pool_size)),
      _is_running(false) {}

void TCPServer::set_callback(Connection::Callback callback) {
  _callback = std::move(callback);
}

void TCPServer::accept() {
  if (!_is_running) return; // If the server isn't running, don't continue accepting clients
  int counter = 0;
  while (_is_running)
  {
    std::cout << "Waiting for connection on port " << _port << "..." << std::endl;

    auto socket = _acceptor.accept();
    std::cout << "Connected to client at " << socket.remote_endpoint().address().to_string() << std::endl;

    auto connection = std::make_shared<Connection>(std::move(socket));
    connection->setCallback(_callback);

    asio::post(_thread_pool->executor(), [connection]() {
      connection->start();
    });

    counter++;
    if (counter >= 10000) {
      stop();
    }
  }
}

void TCPServer::start() {
  _is_running = true;

  std::cout << "Serving TCP connections on port "  << _acceptor.local_endpoint().port() << std::endl;
  _acceptor_thread = std::make_unique<std::thread>(&TCPServer::accept, this);

}

void TCPServer::stop() {
  _is_running = false;
  if (_acceptor_thread->joinable()) {
    _acceptor_thread->join();
  }
  _thread_pool->join();
}

bool TCPServer::is_running() const {
  return _is_running;
}
