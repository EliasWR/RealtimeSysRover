#include "tcp_server/websocket_tcp_server.hpp"


WSConnection::WSConnection(tcp::socket socket)
    : _ws(std::move(socket))
{}

void WSConnection::run()
{
    _thread = std::jthread([&] {
        try
        {
            while (true)
            {
                std::cout << "Waiting for message..." << std::endl;
                auto msg = receiveMessage();
                std::string response{};
                _callback(msg, response);
                writeMsg(response);
            }
        }
        catch (const std::exception& ex)
        {
            std::cerr << "[socket_handler] " << ex.what() << std::endl;
        }
    });

    auto _ = _thread.get_id();
}


void WSConnection::setCallback(Callback& callback)
{
    _callback = callback;
}


int WSConnection::receiveMessageSize()
{
    beast::flat_buffer buffer;
    _ws.read(buffer);
    std::stringstream ss;
    ss << beast::make_printable(buffer.data());
    std::string request = ss.str();
    std::cout << "Received message: " << request << std::endl;
    return bytes_to_int(buf);


    return bytes_to_int(buf);
}


void WS_TCPServer::accept()
{
    if (!_is_running)
    {
        return;
    }
    std::cout << "Websocket TCP Server running on port: " << _port << std::endl;

    _acceptor_thread = std::jthread([&] {
        std::vector<std::unique_ptr<Connection>> connections;
        try
        {
            while (_is_running)
            {
                auto socket = std::make_unique<tcp::socket>(_io_context);
                _acceptor.accept(*socket); //blocking operation

                std::cout << "Accepted connection from: " << socket->remote_endpoint().address().to_string() << std::endl;

                auto connection = std::make_unique<WSConnection>(std::move(*socket));
                std::cout << "Created connection" << std::endl;
                connection->setCallback(_callback);
                std::cout << "Set callback" << std::endl;
                connection->run();
                std::cout << "Started connection" << std::endl;
                connections.push_back(std::move(connection));
            }
        }
        catch (const boost::system::system_error& e)
        {
            std::cerr << "Exception while accepting (Boost system error): " << e.what() << "\n";
        }
        catch (const std::exception& e)
        {
            std::cerr << "Exception while accepting: " << e.what() << "\n";
        }

    });
    }


