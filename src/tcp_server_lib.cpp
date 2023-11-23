#include <utility>

#include "tcp_server/tcp_server_lib.hpp"

// Connection Implementation
Connection::Connection(tcp::socket socket, std::shared_ptr<MessageHandler> handler) :
    _socket(std::move(socket)) {
    _message_handler = std::move(handler);
}


void Connection::run() {
  _thread = std::thread([&] {
    try {
      while (true) {
        auto msg = receiveMessage();
        auto msg_vec = std::vector<char>(msg.begin(), msg.end());
        _message_handler->handleMessage(msg_vec, static_cast<int>(0));
      }
    } catch (const std::exception &ex) {
      std::cerr << "[socket_handler] " << ex.what() << std::endl;
    }
  });
}

void Connection::set_message_handler(std::shared_ptr<MessageHandler> handler) {
  _message_handler = std::move(handler);
}

std::string Connection::getIPv4() {
  return _socket.remote_endpoint().address().to_string();
}

int Connection::receiveMessageSize() {
  std::array<unsigned char, 4> buf{};
  boost::asio::read(_socket, boost::asio::buffer(buf), boost::asio::transfer_exactly(4));
  return bytes_to_int(buf);
}

std::string Connection::receiveMessage() {


  int len = receiveMessageSize();
  boost::asio::streambuf buf;
  boost::system::error_code err;
  //asio::read(*_socket, buf, err);

  boost::asio::read(_socket, buf, boost::asio::transfer_exactly(len), err);

  if (err) {
    throw boost::system::system_error(err);
  }
  //std::string data(boost::asio::buffer_cast<const char *>(buf.data()));


  std::string data(boost::asio::buffer_cast<const char *>(buf.data()), len);
  return data;
}

void Connection::writeMsg(const std::string &msg) {
  int msgSize = static_cast<int>(msg.size());

  _socket.send(boost::asio::buffer(int_to_bytes(msgSize), 4));
  _socket.send(boost::asio::buffer(msg));

}

Connection::~Connection() {
  _socket.close();
  _thread.join();
}

// TCPServer Implementation
TCPServer::TCPServer(int port, std::unique_ptr<MessageHandler> handler) :
    _port(port),
    _acceptor(_io_context, tcp::endpoint(tcp::v4(), port)),
    _is_running(false),
    _message_handler(std::move(handler)) {
}

void TCPServer::start() {
  _is_running = true;
  std::cout << "TCP Server running on port: " << _port << std::endl;

  _acceptor_thread = std::thread([&] {
    try {
      while (_is_running) {

        auto socket = tcp::socket(_io_context);
        _acceptor.accept(socket);

        std::cout << "TCP Connection accepted with: " << socket.remote_endpoint().address().to_string() << std::endl;

        auto connection = std::make_unique<Connection>(std::move(socket), _message_handler);
        _clients.push_back(std::move(connection));
        _clients.back()->set_message_handler(_message_handler);
        _clients.back()->run();

      }
    } catch (const boost::system::system_error &e) {
        std::cerr << "[TCPServer] Exception while accepting (Boost system error): " << e.code() << std::endl;
    } catch (const std::exception &e) {
        std::cerr << "[TCPServer] Exception while accepting: " << e.what() << std::endl;
    }
  });
}


void TCPServer::stop() {
  if (_is_running) {
      std::cout << "Stopping TCP Server..\n";
      _is_running = false;

      _acceptor.cancel();
      _acceptor.close();
      _acceptor_thread.join();

      {
        std::unique_lock<std::mutex> lock(_m);
        for (auto &client : _clients) {
          client->~Connection();
        }
        _clients.clear();
      }

      _io_context.stop();

      std::cout << "DONE" << std::endl;
  }
}

void TCPServer::writeToClient(size_t client_index, const std::string &msg) {
  if (_clients.size() >= client_index + 1) {
    std::cout << "Writing to client " << client_index << ": " << msg << std::endl;
    _clients[client_index]->writeMsg(msg);
  } else {
    throw std::out_of_range("Client index out of range");
  }
}

void TCPServer::writeToAllClients(const std::string &msg) {
  try {
    for (size_t i = 0; i < _clients.size(); i++) {
      std::cout << "Writing to client " << i << ": " << msg << std::endl;
      _clients[i]->writeMsg(msg);
    }
  }
    catch (const std::exception &e) {
        std::cerr << "Exception while writing to all clients: " << e.what() << std::endl;
    }
}
