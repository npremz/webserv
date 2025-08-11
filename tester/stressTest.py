import threading
import requests
import random
import time
import socket
from concurrent.futures import ThreadPoolExecutor

 
SERVER_ADDRESS = "http://127.0.0.1:8060/"
HOST = "127.0.0.1"
PORT = 8060

def getRequestLargeNb(nbReq):

    print(f"Sending {nbReq} request to the server")
    successCount = 0
    failCount = 0
    for i in range(nbReq):
        response = requests.get(SERVER_ADDRESS + "/images/tung-tung.gif")
        if response.status_code == 200:
            successCount += 1
        else:
            failCount += 1
    print(f"Success get:    \033[32m{successCount}\033[0m")
    print(f"Fail get:       \033[31m{failCount}\033[0m\n")

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

def getRequestBadUrl():
    print("Trying bad urls :")
    response = requests.get(SERVER_ADDRESS + "/ThisUrlDoesNotExist")
    if response.status_code != 200:
        print(f"/ThisUrlDoesNotExist: \033[32m{response.status_code}\033[0m")
    else:
        print(f"/ThisUrlDoesNotExist: \033[31m{response.status_code}\033[0m")
    time.sleep(0.1)
    response = requests.get(SERVER_ADDRESS + "/d/////d//")
    if response.status_code != 200:
        print(f"/d/////d//: \033[32m{response.status_code}\033[0m")
    else:
        print(f"/d/////d//: \033[31m{response.status_code}\033[0m")
    time.sleep(0.1)
    response = requests.get(SERVER_ADDRESS + "IMGONNAEXPLODE")
    if response.status_code != 200:
        print(f"IMGONNAEXPLODE: \033[32m{response.status_code}\033[0m")
    else:
        print(f"IMGONNAEXPLODE: \033[31m{response.status_code}\033[0m")
    time.sleep(0.1)
    response = requests.get(SERVER_ADDRESS + "         ")
    if response.status_code != 200:
        print(f"         : \033[32m{response.status_code}\033[0m")
    else:
        print(f"         : \033[31m{response.status_code}\033[0m")
    time.sleep(0.1)
    response = requests.get(SERVER_ADDRESS + "/|\\")
    if response.status_code != 200:
        print(f"/|\\: \033[32m{response.status_code}\033[0m")
    else:
        print(f"/|\\: \033[31m{response.status_code}\033[0m")
    time.sleep(0.1)
    response = requests.get(SERVER_ADDRESS + "\\\\")
    if response.status_code != 200:
        print(f"\\\\: \033[32m{response.status_code}\033[0m")
    else:
        print(f"\\\\: \033[31m{response.status_code}\033[0m")
    time.sleep(0.1)
    response = requests.get(SERVER_ADDRESS + "c")
    if response.status_code != 200:
        print(f"1char url: \033[32m{response.status_code}\033[0m")
    else:
        print(f"1char url: \033[31m{response.status_code}\033[0m")
    time.sleep(0.1)
    response = requests.get(SERVER_ADDRESS + "cccccccccc")
    if response.status_code != 200:
        print(f"10char url: \033[32m{response.status_code}\033[0m")
    else:
        print(f"10char url: \033[31m{response.status_code}\033[0m")
    time.sleep(0.1)
    response = requests.get(SERVER_ADDRESS + "cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc")
    if response.status_code != 200:
        print(f"100char url: \033[32m{response.status_code}\033[0m")
    else:
        print(f"100char url: \033[31m{response.status_code}\033[0m")
    time.sleep(0.1)
    response = requests.get(SERVER_ADDRESS + "cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc")
    if response.status_code != 200:
        print(f"1000char url: \033[32m{response.status_code}\033[0m")
    else:
        print(f"1000char url: \033[31m{response.status_code}\033[0m")
    time.sleep(0.1)
    status_code = getRequestCode(b"GET HTTP/1.1\r\nHost: localhost\r\n\r\n")
    if status_code is not None and status_code != 200:
        print(f"Missing URI: \033[32m{status_code}\033[0m")
    else:
        print(f"Missing URI: \033[31m{status_code}\033[0m")
    time.sleep(0.1)
    status_code = getRequestCode(b"GET /index.html HTTP/1.1!!\r\nHost: localhost\r\n\r\n")
    if status_code is not None and status_code != 200:
        print(f"Garbage characters in request line: \033[32m{status_code}\033[0m")
    else:
        print(f"Garbage characters in request line: \033[31m{status_code}\033[0m")
    time.sleep(0.1)
    status_code = getRequestCode(b"GET / HTTP/1.1\r\nHost: loca\0lhost\r\n\r\n")
    if status_code is not None and status_code != 200:
        print(f"Header with illegal characters: \033[32m{status_code}\033[0m")
    else:
        print(f"Header with illegal characters: \033[31m{status_code}\033[0m")
    time.sleep(0.1)
    status_code = getRequestCode(b"GET / HTTP/1.1\r\nHost: localhost\r\n")
    if status_code is not None and status_code != 200:
        print(f"Incomplete request (simulate timeout): \033[32m{status_code}\033[0m")
    else:
        print(f"Incomplete request (simulate timeout): \033[31m{status_code}\033[0m")

def send_multiple_timeout():
    bad_req = b"GET / HTTP/1.1\r\nHost: localhost\r\n"
    code = getRequestCode(bad_req)
    return code

def thread_function(func, nbWorker, code):
    count = 0
    with ThreadPoolExecutor(max_workers=nbWorker) as executor:
        futures = [executor.submit(func) for _ in range(nbWorker)]
        for f in futures:
            if (f.result() == code):
                count += 1
    return (count)
    
def timeout_sim(func, nbWorker, code):
    print(f"{nbWorker} timeout simulation:")
    count = thread_function(func, nbWorker, code)
    if (count == nbWorker):
        print(f"{count}/1000 \033[32mSuccess\033[0m")
    else:
        print(f"{count}/1000 \033[31mFailure\033[0m")

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
    print("Tests for GET: 10, 100 and 1000 requests")
    time.sleep(1)
    getRequestLargeNb(10)
    time.sleep(1)
    getRequestLargeNb(100)
    time.sleep(1)
    getRequestLargeNb(1000)
    time.sleep(1)
    getRequestBadUrl()
    timeout_sim(send_multiple_timeout, 1000, 408)
if __name__ == '__main__':
    main()
