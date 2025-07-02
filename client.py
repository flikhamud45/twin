from enum import Enum
import socket
import os

MESSAGE_SIZE_SIZE = 2
FILE_SIZE_SIZE = 4
IP = "127.0.0.1"
PORT = 12345
CHUNK_SIZE = 4096

class Command(Enum):
	PING = "ping"
	RUN = "run"
	UPLOAD = "upload"

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

def send_file(sock: socket.socket, file_path: str) -> None:
	"""Send a file over the socket."""
	# send the file name
	file_name = os.path.basename(file_path)
	if not os.path.isfile(file_path):
		raise FileNotFoundError(f"File not found: {file_path}")
	send_message(sock, file_name)

	# send the file size
	file_size = os.path.getsize(file_path)
	if file_size > (1 << (FILE_SIZE_SIZE * 8)) - 1:
		raise ValueError("File too large")
	size_bytes = file_size.to_bytes(FILE_SIZE_SIZE, 'big')
	sock.sendall(size_bytes)

	# send the file content in chunks
	with open(file_path, 'rb') as f:
		while True:
			chunk = f.read(CHUNK_SIZE)
			if not chunk:
				break
			sock.sendall(chunk)
		

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
			case Command.UPLOAD:
				file_path = input("Enter path to file: ").strip()
				if not file_path:
					print("Path cannot be empty")
					continue
				msg = f"{Command.UPLOAD.value}"
				send_message(sock, msg)
				send_file(sock, file_path)
			case _:
				print(f"Unknown command: {command}")
				continue
		print(f"Sent message: {msg}")
		
		msg = receive_message(sock)
		print(f"Received message: {msg}")



if __name__ == "__main__":
	main()
