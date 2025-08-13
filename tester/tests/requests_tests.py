from config import SERVER_ADDRESS
import requests
import time
from .socket_tests import getRequestCode

def getRequestLargeNb(nbReq):
    print(f"\033[1mSending {nbReq} request to the server\033[0m")
    successCount = 0
    failCount = 0
    for i in range(nbReq):
        response = requests.get(SERVER_ADDRESS + "/images/tung-tung.gif")
        if response.status_code == 200:
            successCount += 1
        else:
            failCount += 1
    print(f"Success:    \033[32m✓ {successCount}\033[0m")
    print(f"Fail:       \033[31m✗ {failCount}\033[0m\n")

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
