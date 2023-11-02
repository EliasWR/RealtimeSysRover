#include "network_helper.hpp"

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/beast/websocket.hpp>
#include <string>
#include <functional>
#include <iostream>
#include <sstream>

namespace beast = boost::beast;
namespace asio = boost::asio;
namespace websocket = beast::websocket;

using tcp = boost::asio::ip::tcp;


class TCPConnection {
private:
  websocket::stream<tcp::socket> _socket;

public:
  using Callback = std::function<void(const std::string& request, std::string& response)>;

  explicit TCPConnection(tcp::socket socket) : _socket(std::move(socket)) {}

  void start() {


    try {
        _socket.accept();
      while(true) {
        beast::flat_buffer buffer;
        _socket.read(buffer);
        on_message(buffer);
      }
    } catch (const beast::system_error& se) {
      if (se.code() != websocket::error::closed)
        std::cerr << "Session Error: " << se.code().message() << std::endl;
    } catch (const std::exception& e) {
      std::cerr << "Session Error: " << e.what() << std::endl;
    }
  }


  void set_callback(Callback callback) {
    _callback = std::move(callback);
  }

  virtual void on_message(beast::flat_buffer& buffer) {
    if (_callback)
    {
      std::stringstream ss;
      ss << beast::make_printable(buffer.data());
      std::string request = ss.str();
      std::string response;
      _callback(request, response);
      _socket.write(asio::buffer(response));
      //std::cout << "Callback, received message: " << beast::make_printable(buffer.data()) << std::endl;
    }
    else{
      std::cout << "No callback, received message: " << beast::make_printable(buffer.data()) << std::endl;
    }
  }

private:
  Callback _callback;
};

class TCPServer {
private:
  asio::io_context ioc_;
  tcp::acceptor acceptor_;
  TCPConnection::Callback _callback;

public:
  explicit TCPServer(unsigned short port)
      : ioc_(1), acceptor_(ioc_, {asio::ip::make_address("0.0.0.0"), port}) {}

  void set_callback(TCPConnection::Callback callback) {
    _callback = std::move(callback);
  }

  void run() {
    std::cout << "Serving TCP connections on port " << acceptor_.local_endpoint().port() << std::endl;
    try {
      while(true) {
        std::cout << "Waiting for connection..." << std::endl;
        tcp::socket socket{ioc_};
        acceptor_.accept(socket); //blocking operation
        std::cout << "Connected to client at " << socket.remote_endpoint().address().to_string() << std::endl;
        std::jthread([&, sock = std::move(socket)]() mutable {
          TCPConnection session(std::move(sock));
          if (_callback){
            session.set_callback(_callback);
          }
          session.start();
        }).detach();
      }
    } catch (const std::exception& e) {
      std::cerr << "Server Error: " << e.what() << std::endl;
    }
  }
};

