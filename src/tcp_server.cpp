#include "tcp_server/tcp_server.hpp"
#include <iostream>
#include <utility>

// MessageHandler Implementation



// Connection Implementation
Connection::Connection(std::unique_ptr<tcp::socket> socket) : _socket(std::move(socket)) {
    _callback = [](const std::string& request, std::string& response) {
        std::cout << "Received request: " << request << std::endl;
        response = "Hello from server!";
    };
}

void Connection::setCallback(Callback& callback) {
  _callback = callback;
}

void Connection::run() {
    _thread = std::jthread([&] { try {
            while (true) {
                std::cout << "Waiting for message..." << std::endl;
                auto msg = receiveMessage();
                std::string response{};
                _callback(msg, response);
                writeMsg(response);
            }
        } catch (const std::exception &ex) {
            std::cerr << "[socket_handler] " << ex.what() << std::endl;
        }
  });

    auto _ = _thread.get_id();
}


std::string Connection::getIPv4() {
    return _socket->remote_endpoint().address().to_string();
}


int Connection::receiveMessageSize() {
    std::array<unsigned char, 4> buf{};
    boost::asio::read(*_socket, boost::asio::buffer(buf), boost::asio::transfer_exactly(4));
    return bytes_to_int(buf);
}


std::string Connection::receiveMessage() {



    int len = receiveMessageSize();
    boost::asio::streambuf buf;
    boost::system::error_code err;
    asio::read(*_socket, buf, err);

    //boost::asio::read(*_socket, buf, boost::asio::transfer_exactly(len), err);

    if (err) {
        throw boost::system::system_error(err);
    }
    std::string data(boost::asio::buffer_cast<const char *>(buf.data()));

    std::cout << "Received message: " << data << std::endl;

    //std::string data(boost::asio::buffer_cast<const char *>(buf.data()), len);
    return data;
}


void Connection::writeMsg(const std::string &msg) {
    int msgSize = static_cast<int>(msg.size());

    _socket->send(boost::asio::buffer(int_to_bytes(msgSize), 4));
    _socket->send(boost::asio::buffer(msg));
}




// TCPServer Implementation
TCPServer_::TCPServer_(unsigned short port):
      _port(port),
      _acceptor(_io_context, tcp::endpoint(tcp::v4(), port)),
      _is_running(false) {}

void TCPServer_::set_callback(Connection::Callback callback) {
  _callback = std::move(callback);
}

void TCPServer_::accept() {
  if (!_is_running)
  {
    return;
  }

  std::cout << "TCP Server running on port: " << _port << std::endl;


  _acceptor_thread = std::jthread([&] {
      std::vector<std::unique_ptr<Connection>> connections;
      try {
          while(_is_running) {

                auto socket = std::make_unique<tcp::socket>(_io_context);
                _acceptor.accept(*socket);

                std::cout << "Accepted connection from: " << socket->remote_endpoint().address().to_string() << std::endl;

                auto connection = std::make_unique<Connection>(std::move(socket));
                std::cout << "Created connection" << std::endl;
                connection->setCallback(_callback);
                std::cout << "Set callback" << std::endl;
                connection->run();
                std::cout << "Started connection" << std::endl;
                connections.push_back(std::move(connection));


          }
      } catch (const boost::system::system_error& e)
      {
         std::cerr << "Exception while accepting (Boost system error): " << e.what() << "\n";
      } catch (const std::exception& e)
      {
        std::cerr << "Exception while accepting: " << e.what() << "\n";
      }
    });

  auto _ = _acceptor_thread.get_id();
}

void TCPServer_::start() {
  _is_running = true;
  accept();
}

void TCPServer_::stop() {
  if (_is_running)
  {
    std::cout << "Stopping TCP Server..";
    _is_running = false;
    _acceptor.close();
    std::cout << "DONE" << std::endl;
  }
}

bool TCPServer_::is_running() const {
  return _is_running;
}



