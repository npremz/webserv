import time
from tests.requests_tests import getRequestLargeNb, getRequestBadUrl
from tests.socket_tests import send_multiple_timeout
from tests.concurrency import timeout_sim

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
