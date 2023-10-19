#include <utility>

#include "tcp_server/tcp_server.hpp"
#include "helpers.hpp"

/*
 *  Connection
 */

Connection::Connection(asio::io_context &ioc)
    : Connection(ioc, [](beast::buffer &buffer) {
        std::cout << "Received: " << beast::make_printable(buffer.data()) << std::endl;
        return std::string(buffer.data());
      }) {
}

Connection::Connection(asio::io_context &ioc, const std::function<std::string()>& handler)
    : _socket(ioc),
      _handler(std::move(handler)){}
}

void Connection::start() {
  _thread = std::jthread([ & ]() {
    try
    {
      _socket.accept();
      while (true)
      {
        auto msg = recvMessage();
        _handler(msg);
      }
    }
    catch (const beast::system_error &e)
    {
      if (e.code() != beast::websocket::error::closed)
        std::cerr << "Session Error: " << e.code().message() << std::endl;
    }
    catch (const std::exception &e)
    {
      std::cerr << "Session Error: " << e.what() << std::endl;
    }
  });

}


void Connection::write(const std::string &msg) {
  int msgSize = static_cast<int>(msg.size());

  _socket.send(boost::asio::buffer(int_to_bytes(msgSize), 4));
  _socket.send(boost::asio::buffer(msg));
}


int Connection::_recieveSize() {
  std::array<unsigned char, 4> buf{};
  boost::asio::read(_socket, boost::asio::buffer(buf), boost::asio::transfer_exactly(4));
  return bytes_to_int(buf);
}

std::string Connection::recieveMessage() {

  int len = _recieveSize();

  boost::asio::streambuf buf;
  boost::system::error_code err;
  boost::asio::read(_socket, buf, boost::asio::transfer_exactly(len), err);

  if (err) {
    throw boost::system::system_error(err);
  }

  std::string data(boost::asio::buffer_cast<const char *>(buf.data()), len);
  return data;
}


Connection::~Connection() {
  if (_thread.joinable())
    _thread.join();
}


/*
 *  TCPServer
 *  For now only accepting ipv4
 */

TCPServer::TCPServer(int port)
    : _port(port),
      _ioc(),
      _acceptor(_ioc, tcp::endpoint(tcp::v4(), port)) {
}


void TCPServer::start() {
  _thread = std::jthread([ & ]() {
    _ioc.run();
    std::vector<std::unique_ptr<Connection>> connections;

    try {
      while (true) {
        auto socket_ptr = std::make_unique<tcp::socket>(_ioc);
        _acceptor.accept(*socket_ptr);

        auto connection_ptr = std::make_unique<Connection>(_ioc);

        connections.emplace_back(std::move(connection_ptr));
        connections.back()->start();
      }
    }

    catch (const std::exception &e) {
      std::cerr << "Server Error: " << e.what() << std::endl;
    }
  });
}

int TCPServer::stop() {
  _stop_all = true;
  if (_thread.joinable()) {
    _thread.join();
    return 0;
  }

  return 0;
}
