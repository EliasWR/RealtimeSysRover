import socket
import cv2
from protobuf.my_messages_pb2 import VideoFeed, Instruction

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
server_address = ('10.22.192.34', 8080)
MAX_UDP_PACKET_SIZE = 65507

cap = cv2.VideoCapture(0)

if not cap.isOpened():
    print("Could not open camera!")
    exit()

while True:
    ret, frame = cap.read()

    if not ret:
        print("Failed to grab frame!")
        break

    cv2.imshow("Webcam", frame)

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
        scale_factor -= 0.1

        if scale_factor <= 0.1:
            print("Frame too large to fit into UDP packet even after reducing resolution.")
            break

    sock.sendto(serialized_video_feed, server_address)

    serialized_instruction, _ = sock.recvfrom(MAX_UDP_PACKET_SIZE)
    instruction = Instruction()
    instruction.ParseFromString(serialized_instruction)

    print("Received instruction:", instruction.messageInstruction)

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
sock.close()
