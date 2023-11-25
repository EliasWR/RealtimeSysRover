import socket
#from protobuf.my_messages_pb2 import VideoFeed, Instruction
import base64
import json
import numpy as np
import cv2
import datetime

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
# server_address = ('10.22.192.34', 8080)
server_address = ('127.0.0.1', 8080)
MAX_UDP_PACKET_SIZE = 65507

'''cap = cv2.VideoCapture(0)

if not cap.isOpened():
    print("Could not open camera!")
    exit()'''

while True:
    #ret, frame = cap.read()
    frame = np.zeros((480, 640, 3), np.uint8)
    cv2.putText(frame, "Hello From UDP!", (100, 100), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 255, 0), 2, cv2.LINE_AA)
    # Add timestamp of HH:MM:SS:MS
    timestamp = datetime.datetime.now().strftime("%H:%M:%S:%f")[:-3]
    cv2.putText(frame, timestamp, (100, 200), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 255, 0), 2, cv2.LINE_AA)

    '''
    if not ret:
        print("Failed to grab frame!")
        break
'''
    cv2.imshow("Webcam", frame)


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
            print("Frame too large to fit into UDP packet even after reducing resolution.")
            break

    sock.sendto(serialized_video_feed, server_address)
    print("Sent frame to server on ", server_address)
    '''
    # 
    try:
        serialized_instruction, _ = sock.recvfrom(MAX_UDP_PACKET_SIZE)
        #instruction = Instruction()
        #instruction.ParseFromString(serialized_instruction)
    except Exception as e:
        print(f"An error occurred: {e}")
        break
    '''

    # print("Received instruction:", instruction.messageInstruction)

    if cv2.waitKey(100) & 0xFF == ord('q'):
        break

#cap.release()
cv2.destroyAllWindows()
sock.close()
