import logging
from websocket_server import WebsocketServer

def client_left(client, server):
    print("Client(%d) disconnected" % client['id'])

def new_client(client, server):
    print("New client connected and was given id %d" % client['id'])

def message_received(client, server, message):
    print("Received command from client(%d): %s" % (client['id'], message))

if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO)
    server = WebsocketServer(host="0.0.0.0", port=12345)
    server.set_fn_new_client(new_client)
    server.set_fn_client_left(client_left)
    server.set_fn_message_received(message_received)
    server.run_forever()
