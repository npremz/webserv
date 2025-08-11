from config import SERVER_ADDRESS
import requests
import time
from .socket_tests import getRequestCode

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


def getRequestBadUrl():
    print("Trying bad urls :")
    urls = [
        "/ThisUrlDoesNotExist",
        "/d/////d//",
        "IMGONNAEXPLODE",
        "         ",
        "/|\\",
        "\\\\",
        "c",
        "cccccccccc",
        "c" * 100,
        "c" * 1000,
    ]
    for u in urls:
        time.sleep(0.1)
        response = requests.get(SERVER_ADDRESS + u)

        if u == "c" * 1000:
            display_url = "1000char"
        elif u == "c" * 100:
            display_url = "100char"
        elif u == "c" * 10:
            display_url = "10char"
        else:
            display_url = u
            
        if response.status_code != 200:
            print(f"{display_url}: \033[32m{response.status_code}\033[0m")
        else:
            print(f"{display_url}: \033[31m{response.status_code}\033[0m")

    time.sleep(0.1)
    tests = [
        (b"GET HTTP/1.1\r\nHost: localhost\r\n\r\n", "Missing URI"),
        (b"GET /index.html HTTP/1.1!!\r\nHost: localhost\r\n\r\n", "Garbage characters in request line"),
        (b"GET / HTTP/1.1\r\nHost: loca\0lhost\r\n\r\n", "Header with illegal characters"),
        (b"GET / HTTP/1.1\r\nHost: localhost\r\n", "Incomplete request (simulate timeout)"),
    ]
    for bad_req, label in tests:
        time.sleep(0.1)
        status_code = getRequestCode(bad_req)
        if status_code is not None and status_code != 200:
            print(f"{label}: \033[32m{status_code}\033[0m")
        else:
            print(f"{label}: \033[31m{status_code}\033[0m")


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
