from enum import Enum
import socket

MESSAGE_SIZE_SIZE = 2
IP = "127.0.0.1"
PORT = 12345

class Command(Enum):
	PING = "ping"
	RUN = "run"

def send_message(sock: socket.socket, message: str) -> None:
	size = len(message)
	if len(message) > 1 << (MESSAGE_SIZE_SIZE * 8)  - 1:
		raise ValueError("Message too long")
	size_bytes = size.to_bytes(MESSAGE_SIZE_SIZE, 'big')
	sock.sendall(size_bytes)
	sock.sendall(message.encode('utf-8'))

def recvall(sock: socket.socket, size: int) -> bytes:
	data = bytearray()
	while len(data) < size:
		chunk = sock.recv(size - len(data))
		if not chunk:
			raise ConnectionError("Connection closed")
		data.extend(chunk)
	return bytes(data)

def receive_message(sock: socket.socket) -> str:
	size_bytes = recvall(sock, MESSAGE_SIZE_SIZE)
	if not size_bytes:
		raise ConnectionError("Connection closed")
	size = int.from_bytes(size_bytes, 'big')
	return recvall(sock, size).decode('utf-8')

def main() -> None:
	sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	sock.connect((IP, PORT))
	print("Connected to server")

	while True:
		command = input("Enter command: ").strip().lower()
		try:
			command = Command(command)
		except ValueError:
			print(f"Unknown command: {command}")
			continue
		msg = ""
		match command:
			case Command.PING:
				msg = Command.PING.value
				send_message(sock, msg)
			case Command.RUN:
				path = input("Enter path to run: ").strip()
				if not path:
					print("Path cannot be empty")
					continue
				msg = f"{Command.RUN.value} {path}"
				send_message(sock, msg)
			case _:
				print(f"Unknown command: {command}")
				continue
		print(f"Sent message: {msg}")
		
		msg = receive_message(sock)
		print(f"Received message: {msg}")



if __name__ == "__main__":
	main()
