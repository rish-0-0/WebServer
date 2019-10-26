import socket
import sys

SERVER_IP = socket.gethostbyname(socket.gethostname())
PORT = 8080
# client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
# client.connect((SERVER_IP, PORT))
for i in range(int(sys.argv[1])):
	client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	# client.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
	client.connect((SERVER_IP, PORT))