import threading
import requests
import random
import time
import socket

 
SERVER_ADDRESS = "http://127.0.0.1:8060/"
HOST = "127.0.0.1"
PORT = 8060

def getRequest():

    successCount = 0
    failCount = 0
    for i in range(100):
        response = requests.get(SERVER_ADDRESS + "/images/tung-tung.gif")
        if response.status_code == 200:
            successCount += 1
        else:
            failCount += 1
    print(f"Success get:    {successCount}")
    print(f"Fail get:       {failCount}")

def postRequest():

    successCount = 0
    failCount = 0
    for i in range(1000):
        data = {'login': 'waf', 'password': 'wafwaf'}
        response = requests.post(SERVER_ADDRESS + "/cgi-bin/login.py", data=data)
        if response.status_code == 200:
            successCount += 1
        else:
            failCount += 1
    print(f"Success post: {successCount}")
    print(f"fail post: {failCount}")

def postRequestSocket():

    BODY = 'login=waf&password=wafwaf'
    HEADERS = [
    f"POST /cgi-bin/login.py HTTP/1.1",
    f"Host: {HOST}:{PORT}",
    f"Content-Length: {len(BODY) + 100}",
    "Content-Type: application/x-www-form-urlencoded",
    "Connection: close",
    "",
    BODY
    ]

    request = "\r\n".join(HEADERS)
    print(request)
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.settimeout(5)

    s.connect((HOST, PORT))
    s.sendall(request.encode())
    response = b""
    try:
        while True:
            chunk = s.recv(4096)
            if not chunk:
                break
            response += chunk
    except socket.timeout:
        print("Timeout client")

    s.close
    print(response.decode(errors='replace'))

def main():
    postRequestSocket()

if __name__ == '__main__':
    main()
