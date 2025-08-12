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


def getRequestBadUrl():
    print("\033[1mGet request with wrong urls\033[0m")
    
    urls_with_expected = [
        ("/ThisUrlDoesNotExist", 404),
        ("/d/////d//", 404),
        ("IMGONNAEXPLODE", 400),
        ("         ", 400),
        ("/|\\", 400),
        ("\\\\", 400),
        ("/c", 404),
        ("/cccccccccc", 404),
        ("/" + "c" * 100, 404),
        ("/" + "c" * 1000, 414),
    ]
    
    # Calculer la largeur maximale pour l'alignement
    max_url_len = max(len("/ThisUrlDoesNotExist"), len("1000char"), len("100char"), len("cccccccccc"))
    
    for u, expected_code in urls_with_expected:
        time.sleep(0.1)
        response = requests.get(SERVER_ADDRESS + u)
        
        if u == ("/" + "c" * 1000):
            display_url = "1000char"
        elif u == ("/" + "c" * 100):
            display_url = "100char"
        else:
            display_url = u
            
        if response.status_code == expected_code:
            print(f"{display_url:<{max_url_len+2}} : \033[32m{response.status_code:3}\033[0m | expected: {expected_code:3} \033[32m✓\033[0m")
        else:
            print(f"{display_url:<{max_url_len+2}} : \033[31m{response.status_code:3}\033[0m | expected: {expected_code:3} \033[31m✗\033[0m")

    time.sleep(0.1)
    
    tests = [
        (b"GET HTTP/1.1\r\nHost: localhost\r\n\r\n", "Missing URI", 400),
        (b"GET /index.html HTTP/1.1!!\r\nHost: localhost\r\n\r\n", "Garbage characters in request line", 400),
        (b"GET / HTTP/1.1\r\nHost: loca\0lhost\r\n\r\n", "Header with illegal characters", 400),
        (b"GET / HTTP/1.1\r\nHost: localhost\r\n", "Incomplete request (simulate timeout)", 408),
    ]
    
    max_label_len = max(len(label) for _, label, _ in tests)
    
    for bad_req, label, expected_code in tests:
        time.sleep(0.1)
        status_code = getRequestCode(bad_req)
        
        status_str = str(status_code) if status_code else 'None'
        
        if status_code == expected_code:
            print(f"{label:<{max_label_len+2}} : \033[32m{status_str:>4}\033[0m | expected: {expected_code:3} \033[32m✓\033[0m")
        else:
            print(f"{label:<{max_label_len+2}} : \033[31m{status_str:>4}\033[0m | expected: {expected_code:3} \033[31m✗\033[0m")
    print("\n")

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
