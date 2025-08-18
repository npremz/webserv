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
    s.send(f"Host: localhost:{PORT}\r\n\r\n")
    response = s.recv(4096).decode(errors="ignore")
    s.close()
    try:
        status_code = int(response.split("\r\n")[0].split(" ")[1])
    except (IndexError, ValueError):
        status_code = None
    return status_code

def test_startline_parser():
    startline_to_try = [
        (b"GET / HTTP/1.1\r\n", 200, "Standard start-line"),
        (b"UNKNOWN / HTTP/1.1\r\n", 501, "Unknown method"),
        (b"get / HTTP/1.1\r\n", 501, "Method in lowercase"),
        (b"GeT / HTTP/1.1\r\n", 501, "Method mixed with lower and uppercase"),
        (b"/ HTTP/1.1\r\n", 400, "No method"),
        (b"GET HTTP/1.1\r\n", 400, "No URI"),
        (b"GET /\r\n", 400, "No HTTP version"),
        (b"GET / HTTP/1.1 EXTRA\r\n", 400, "Too many elements"),
        (b"GET    /    HTTP/1.1\r\n", 200, "Spaces in start-line"),
        (b"GET\t/\tHTTP/1.1\r\n", 200, "Tabs replaces spaces"),
        ((b"GET /" + b"A" * 5000 + b" HTTP/1.1\r\n"), 414, "Start-line too long"),
        (b"GET /../Makefile HTTP/1.1\r\n", 403, "Path traversal"),
        (b"GET /. HTTP/1.1\r\n", 403, "Path with \"/.\""),
        (b"HEAD / HTTP/1.1\r\n", 501, "Method not implemented"),
        (b"GET / HTTP/2.0\r\n", 400, "HTTP version 2.0"),
        (b"GET / HTTP1.1\r\n", 400, "Invalid HTTP version format (no slash)"),
        (b"GET / HTTTP/1.1\r\n", 400, "Invalid HTTP version format (wrong prefix)"),
        (b"GET / HTTP/1\r\n", 400, "HTTP version too short"),
        (b"GET / HTTP/1.11\r\n", 400, "HTTP version too long"),
        (b"GET ///////// HTTP/1.1\r\n", 200, "URI with multiple consecutive slashes"),
        (b"GET /index.html?param=value&foo=bar HTTP/1.1\r\n", 200, "URI with query string"),
        (b"GET /page#section HTTP/1.1\r\n", 400, "URI with fragment"),
        (b"GET /%2e%2e/Makefile HTTP/1.1\r\n", 404, "URI with encoded characters"),
        (b"GET /\x00 HTTP/1.1\r\n", 400, "URI with control characters"),
        (b"GET / index.php HTTP/1.1\r\n", 400, "Space in URI")
        

    ]
    
    print("\033[1mTesting start-line parsing:\033[0m")
    
    for test, expected_code, description in startline_to_try:
        time.sleep(0.1)
        request = (test + b"Host: localhost:8060\r\n\r\n")
        status_code = getRequestCode(request)
        status_str = str(status_code) if status_code else 'None'
        
        if status_code == expected_code:
            print(f"{description:45} : \033[32m{status_str:>3}\033[0m | expected: {expected_code:3} \033[32m✓\033[0m")
        else:
            print(f"{description:45} : \033[31m{status_str:>5}\033[0m | expected: {expected_code:3} \033[31m✗\033[0m")

def test_headers_parser():
    host_headers_to_try = [
        (f"Host: localhost:{PORT}\r\nContent-Length: 100\r\nContent-Length: 200\r\n".encode(), 400, "Duplicate Content-Length"),
        (f"Host: localhost:{PORT}\r\nHost: evil.com:{PORT}\r\n".encode(), 400, "Duplicate Host header"),
        (b"Host: localhost\r\n", 400, "Host without port"),
        (b"Host: localhost:9999\r\n", 412, "Wrong port number"),
        ((f"Host: localhost:{PORT}\r\n".encode() + b"X-Test: value\r\n" * 1050), 400, "Too many headers (1050)"),
        (f"Host: localhost:{PORT}\r\nThis is invalid header\r\n".encode(), 400, "Invalid header format"),
        (f"Host: localhost:{PORT}\r\nContent-Length: 70000000\r\n".encode(), 413, "Content-Length too large"),
        (f"Host: localhost:{PORT}\r\nX-Bad: value\x01here\r\n".encode(), 400, "Control char in header"),
        (f"Host: localhost:{PORT}\r\nX Bad Header: value\r\n".encode(), 400, "Space in header name"),
        (f"Host: localhost:{PORT}\r\nX-Time: 12:34:56\r\n".encode(), 200, "Valid custom header"),
        (f"Host: localhost:{PORT}\r\n: value\r\n".encode(), 200, "Empty header name"),
        (f"Host: localhost:{PORT}\r\n	X-Spaces  :  value   \r\n".encode(), 200, "Spaces around header"),
        (f"Host: localhost:{PORT}\r\nX-Mixed: value\n".encode(), 400, "LF instead of CRLF"),
        (f"Host: localhost:{PORT}\r\nX-Folded: first line\r\n continued line\r\n".encode(), 400, "Folded header line"),
        (f"Host: localhost:{PORT}\r\nContent-Length: -5\r\n".encode(), 400, "Negative Content-Length"),
        (f"Host: localhost:{PORT}\r\nContent-Length: 12abc\r\n".encode(), 400, "Non-numeric Content-Length"),
        (f"Host: localhost:{PORT}\r\nContent-Length: 99999999999999999999\r\n".encode(), 413, "Overflow Content-Length"),
        (f"HoSt: localhost:{PORT}\r\ncontent-LENGTH: 0\r\nTRANSFER-encoding: identity\r\n".encode(), 200, "Mixed case headers"),
        ((f"Host: localhost:{PORT}\r\nX-Long: ".encode() + b"A" * 8200 + b"\r\n"), 431, "Header value too long"),
        (f"Host: 999.999.999.999:{PORT}\r\n".encode(), 200, "Invalid IP in Host"),
        (b"Host: localhost:99999\r\n", 400, "Port out of range"),
        (b"\r\n", 400, "Missing Host header")
    ]
    
    print("\033[1mTesting header parsing:\033[0m")
    
    for test, expected_code, description in host_headers_to_try:
        time.sleep(0.1)
        request = (b"GET / HTTP/1.1\r\n" + test + b"\r\n")
        status_code = getRequestCode(request)
        status_str = str(status_code) if status_code else 'None'
        
        if status_code == expected_code:
            print(f"{description:25} : \033[32m{status_str:>3}\033[0m | expected: {expected_code:3} \033[32m✓\033[0m")
        else:
            print(f"{description:25} : \033[31m{status_str:>5}\033[0m | expected: {expected_code:3} \033[31m✗\033[0m")

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
        ('/' + 'c' * 100, 404),
        ('/' + 'c' * 5000, 414),
    ]

    max_url_len = max(len("/ThisUrlDoesNotExist"), len("1000char"), len("100char"), len("cccccccccc"))
    
    for u, expected_code in urls_with_expected:
        time.sleep(0.1)
        code = getRequestCode(f"GET {u} HTTP/1.1\r\nHost: localhost:8060\r\n\r\n".encode())

        if u == ('/' + 'c' * 5000):
            display_url = "5000char"
        elif u == ('/' + 'c' * 100):
            display_url = "100char"
        else:
            display_url = u
            
        if code == expected_code:
            print(f"{display_url:<{max_url_len+2}} : \033[32m{code:3}\033[0m | expected: {expected_code:3} \033[32m✓\033[0m")
        else:
            print(f"{display_url:<{max_url_len+2}} : \033[31m{code:3}\033[0m | expected: {expected_code:3} \033[31m✗\033[0m")

    time.sleep(0.1)
    
    tests = [
        (b"GET HTTP/1.1\r\nHost: localhost\r\n\r\n", "Missing URI", 400),
        (b"GET /index.html HTTP/1.1!!\r\nHost: localhost:8060\r\n\r\n", "Garbage characters in request line", 400),
        (b"GET / HTTP/1.1\r\nHost: local\0\r\1\2\n\n\n\nhost:8060\r\n\r\n", "Header with illegal characters", 400),
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
