import socket

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

sock.connect(("localhost", 6379))

command = (
    "*3\r\n"
    "$3\r\n"
    "SET\r\n"
    "$4\r\n"
    "name\r\n"
    "$6\r\n"
    "Anshul\r\n"
)

sock.sendall(command.encode())

response = sock.recv(4096)

print(response.decode())

sock.close()