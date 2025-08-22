import time
from tests.requests_tests import getRequestLargeNb
from tests.socket_tests import send_multiple_timeout, test_headers_parser, test_startline_parser, test_body_parser, getRequestBadUrl
from tests.concurrency import timeout_sim, test_concurrent_delete

def main():
    wait = 1
    # time.sleep(wait)
    # test_startline_parser()
    # time.sleep(wait)
    # test_headers_parser()
    time.sleep(wait)
    test_body_parser()
    # time.sleep(wait)
    # getRequestLargeNb(10)
    # time.sleep(wait)
    # getRequestLargeNb(100)
    # time.sleep(wait)
    # getRequestLargeNb(1000)
    # time.sleep(wait)
    # getRequestBadUrl()
    # time.sleep(wait)
    # timeout_sim(send_multiple_timeout, 1000, 408)
    # time.sleep(wait)
    # test_concurrent_delete("/cgi-bin/uploads/test.txt", 50, 404)

if __name__ == '__main__':
    main()
