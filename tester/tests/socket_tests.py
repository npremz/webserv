from config import HOST, PORT
import socket

def getRequestCode(bad_req):
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((HOST, PORT))
    s.sendall(bad_req)
    response = s.recv(4096).decode(errors="ignore")
    s.close()
    try:
        status_code = int(response.split("\r\n")[0].split(" ")[1])
    except (IndexError, ValueError):
        status_code = None
    return status_code


def send_multiple_timeout():
    bad_req = b"GET / HTTP/1.1\r\nHost: localhost\r\n"
    code = getRequestCode(bad_req)
    return code
