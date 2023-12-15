# An initial sketch was inspired by Networking Demo by Lars Ivar Hatledal:
# https://github.com/AIS2203-H-2023/networking_demo/blob/master/tcp/python/py_tcp_client.py

import socket
import base64
import json
import numpy as np
import cv2
import datetime

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
# server_address = ('10.22.192.34', 8080)
server_address = ('127.0.0.1', 8080)
MAX_UDP_PACKET_SIZE = 65507

while True:
    frame = np.zeros((480, 640, 3), np.uint8)
    cv2.putText(frame, "Hello From udp!", (100, 100), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 255, 0), 2, cv2.LINE_AA)
    timestamp = datetime.datetime.now().strftime("%H:%M:%S:%f")[:-3]
    cv2.putText(frame, timestamp, (100, 200), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 255, 0), 2, cv2.LINE_AA)

    cv2.imshow("Python", frame)

    scale_factor = 1.0
    while True:
        dim = (int(frame.shape[1] * scale_factor), int(frame.shape[0] * scale_factor))
        resized_frame = cv2.resize(frame, dim, interpolation=cv2.INTER_AREA)

        is_success, buffer = cv2.imencode(".jpg", resized_frame)

        if not is_success:
            print("Failed to encode image!")
            break
        
        encoded_image = base64.b64encode(buffer).decode('utf-8')
  
        video_feed = {"image": encoded_image}

        
        serialized_video_feed = json.dumps(video_feed).encode("utf-8")
     
        if len(serialized_video_feed) <= MAX_UDP_PACKET_SIZE:
            break
        scale_factor -= 0.1

        if scale_factor <= 0.1:
            print("Frame too large to fit into udp packet even after reducing resolution.")
            break

    sock.sendto(serialized_video_feed, server_address)

    if cv2.waitKey(100) & 0xFF == ord('q'):
        break

cv2.destroyAllWindows()
sock.close()
