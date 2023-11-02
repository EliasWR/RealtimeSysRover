#ifndef REALTIMESYSROVER_WEBSOCKET_TCP_SERVER_HPP
#define REALTIMESYSROVER_WEBSOCKET_TCP_SERVER_HPP

#include "tcp_server.hpp"
#include <iostream>

namespace websocket = beast::websocket;

class WSConnection {
    public:
      explicit WSConnection(tcp::socket socket);
      void run();
      //using Callback = std::function<void(Connection& conn, const std::string& request, std::string& response)>;
      using Callback = std::function<void(const std::string& request, std::string& response)>;

      void setCallback(Callback& callback);
      int receiveMessageSize();
      std::string receiveMessage();
      void writeMsg(const std::string& msg);

    protected:
      websocket::stream<beast::tcp_stream>  _ws;
      Callback _callback;
      std::jthread _thread;

};



class WS_TCPServer : private TCPServer_ {
    void accept() override;
};




#endif // REALTIMESYSROVER_WEBSOCKET_TCP_SERVER_HPP
