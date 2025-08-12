from config import HOST, PORT
import socket
import time

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

def concurrent_delete(path):
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((HOST, PORT))
    s.send(f"DELETE {path} HTTP/1.1\r\n".encode())
    s.send(b"Host: localhost:8060\r\n\r\n")
    response = s.recv(4096).decode(errors="ignore")
    s.close()
    try:
        status_code = int(response.split("\r\n")[0].split(" ")[1])
    except (IndexError, ValueError):
        status_code = None
    return status_code

def test_host_parser():
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((HOST, PORT))
    s.send(b"GET / HTTP/1.1\r\n")
    s.send(b"Host: localhost:8060\r\n")
    s.send(b"Host: evil.com:8060\r\n")
    s.send(b"Content-Length: 100\r\n")
    s.send(b"Content-Length: 200\r\n\r\n")
    response = s.recv(4096).decode(errors="ignore")
    s.close()
    try:
        status_code = int(response.split("\r\n")[0].split(" ")[1])
    except (IndexError, ValueError):
        status_code = None
    print(status_code)

def test_path_traversal():
    paths_with_expected = [
        ("/../Makefile", 403),
        ("/%2e%2e/%2e%2e/", 404),
        ("/./././../../../etc/passwd", 403),
        ("//../../etc/passwd", 403),
    ]

    print("\033[1mTrying to break into the server with paths:\033[0m")    
    max_path_len = max(len(path) for path, _ in paths_with_expected)
    for path, expected_code in paths_with_expected:
        time.sleep(0.1)
        request = f"GET {path} HTTP/1.1\r\nHost: localhost:8060\r\n\r\n".encode()
        status_code = getRequestCode(request)
        status_str = str(status_code) if status_code else 'None'
        if status_code == expected_code:
            print(f"Path {path:<{max_path_len}} : \033[32m{status_str:>3}\033[0m | expected: {expected_code:3} \033[32m✓\033[0m")
        else:
            print(f"Path {path:<{max_path_len}} : \033[31m{status_str:>4}\033[0m | expected: {expected_code:3} \033[31m✗\033[0m")
