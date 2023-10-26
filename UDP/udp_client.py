import socket

from protobuf.my_messages_pb2 import VideoFeed, Instruction  # Generated from protoc

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
server_address = ('127.0.0.1', 8080)
MAX_UDP_PACKET_SIZE = 65507  # Maximum UDP packet size

while True:
    # Capture video frame
    video_feed = VideoFeed()
    video_feed.messageFeed = "This is a test message from the client."

    # Serialize video frame message
    serialized_video_feed = video_feed.SerializeToString()

    # Send video frame to server
    sock.sendto(serialized_video_feed, server_address)

    # Receive instruction from server
    serialized_instruction, _ = sock.recvfrom(MAX_UDP_PACKET_SIZE)
    instruction = Instruction()
    instruction.ParseFromString(serialized_instruction)

    # Execute instruction on robot
    # Print received instruction
    print("Received instruction:", instruction.messageInstruction)

sock.close()
