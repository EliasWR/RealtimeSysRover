import socket
import cv2
import numpy as np

from protobuf.my_messages_pb2 import VideoFeed, Instruction  # Generated from protoc

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
server_address = ('127.0.0.1', 8080)
MAX_UDP_PACKET_SIZE = 65507  # Maximum UDP packet size

# Initialize webcam
cap = cv2.VideoCapture(0)

if not cap.isOpened():
    print("Could not open webcam!")
    exit(0)

while True:
    ret, frame = cap.read()  # Read a frame from the webcam

    if not ret:
        print("Failed to grab frame!")
        break

    # Display the frame for debugging purposes
    cv2.imshow("Webcam", frame)

    # Reduce resolution until it fits in UDP packet
    scale_factor = 1.0
    while True:
        dim = (int(frame.shape[1] * scale_factor), int(frame.shape[0] * scale_factor))
        resized_frame = cv2.resize(frame, dim, interpolation=cv2.INTER_AREA)
        is_success, buffer = cv2.imencode(".jpg", resized_frame)

        if not is_success:
            print("Failed to encode image!")
            break

        video_feed = VideoFeed()
        video_feed.messageFeed = buffer.tobytes()
        serialized_video_feed = video_feed.SerializeToString()

        if len(serialized_video_feed) <= MAX_UDP_PACKET_SIZE:
            break
        scale_factor -= 0.1  # Reduce scale by 10%

        if scale_factor <= 0.1:  # Avoid endless loop and very low resolution
            print("Frame too large to fit into UDP packet even after reducing resolution.")
            break

    # Send video frame to server
    sock.sendto(serialized_video_feed, server_address)

    # Receive instruction from server
    serialized_instruction, _ = sock.recvfrom(MAX_UDP_PACKET_SIZE)
    instruction = Instruction()
    instruction.ParseFromString(serialized_instruction)

    # Execute instruction on robot
    # Print received instruction
    print("Received instruction:", instruction.messageInstruction)

    # Break the loop if 'q' is pressed
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# Release resources
cap.release()
cv2.destroyAllWindows()
sock.close()
