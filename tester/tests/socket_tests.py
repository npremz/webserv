from config import HOST, PORT
import socket
import time

def getRequestCode(bad_req):
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.settimeout(30)
        
        try:
            s.connect((HOST, PORT))
        except ConnectionRefusedError:
            print(f"Error: Cannot connect to server at {HOST}:{PORT} - Server may not be running")
            return None
        except socket.timeout:
            print(f"Error: Connection timeout to {HOST}:{PORT}")
            return None
        except socket.error as e:
            print(f"Error: Connection failed - {e}")
            return None
        
        try:
            s.sendall(bad_req)
        except ConnectionResetError:
            print(f"Error: Connection reset by server (request may have crashed the server)")
            return None
        except BrokenPipeError:
            print(f"Error: Broken pipe (server closed connection)")
            return None
        except socket.error as e:
            print(f"Error: Failed to send request - {e}")
            return None
        
        try:
            response = s.recv(4096).decode(errors="ignore")
        except socket.timeout:
            print(f"Error: Server did not respond within timeout")
            return None
        except ConnectionResetError:
            print(f"Error: Server reset connection while receiving response")
            return None
        except socket.error as e:
            print(f"Error: Failed to receive response - {e}")
            return None
        
        s.close()
        
        if not response:
            print("Error: Server sent empty response")
            return None
        
        try:
            status_line = response.split("\r\n")[0] if response else "empty"
            status_code = int(response.split("\r\n")[0].split(" ")[1])
        except (IndexError, ValueError):
            print(f"Error: Invalid HTTP response format: {status_line}")
            status_code = None
            
        return status_code
        
    except Exception as e:
        print(f"Unexpected error: {e}")
        if 's' in locals():
            s.close()
        return None


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
        request = (test + f"Host: localhost:{PORT}\r\n\r\n".encode())
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
        ((f"Host: localhost:{PORT}\r\n".encode() + b"X-Test: value\r\n" * 3150), 400, "Too many headers (3150)"),
        (f"Host: localhost:{PORT}\r\nThis is invalid header\r\n".encode(), 400, "Invalid header format"),
        (f"Host: localhost:{PORT}\r\nContent-Length: 70000000\r\n".encode(), 413, "Content-Length too large"),
        (f"Host: localhost:{PORT}\r\nX-Bad: value\x01here\r\n".encode(), 400, "Control char in header"),
        (f"Host: localhost:{PORT}\r\nX Bad Header: value\r\n".encode(), 400, "Space in header name"),
        (f"Host: localhost:{PORT}\r\nX-Time: 12:34:56\r\n".encode(), 200, "Valid custom header"),
        (f"Host: localhost:{PORT}\r\n: value\r\n".encode(), 200, "Empty header name"),
        (f"Host: localhost:{PORT}\r\n	X-Spaces  :  value   \r\n".encode(), 200, "Spaces around header"),
        (f"Host: localhost:{PORT}\r\nX-Mixed: value\n".encode(), 408, "LF instead of CRLF"),
        (f"Host: localhost:{PORT}\r\nX-Folded: first line\r\n continued line\r\n".encode(), 400, "Folded header line"),
        (f"Host: localhost:{PORT}\r\nContent-Length: -5\r\n".encode(), 400, "Negative Content-Length"),
        (f"Host: localhost:{PORT}\r\nContent-Length: 12abc\r\n".encode(), 400, "Non-numeric Content-Length"),
        (f"Host: localhost:{PORT}\r\nContent-Length: 99999999999999999999\r\n".encode(), 413, "Overflow Content-Length"),
        (f"HoSt: localhost:{PORT}\r\ncontent-LENGTH: 0\r\nTRANSFER-encoding: identity\r\n".encode(), 200, "Mixed case headers"),
        ((f"Host: localhost:{PORT}\r\nX-Long: ".encode() + b"A" * 16400 + b"\r\n"), 431, "Header value too long"),
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

def test_body_parser():
    host_headers_to_try = [
        (b"Content-Length: 5\r\n\r\nhello\r\n\r\n", 200, "Valid Content-Length body"),
        (b"Content-Length: 10\r\n\r\nhello\r\n\r\n", 408, "Body shorter than Content-Length"),
        (b"Content-Length: 5\r\n\r\nhelloEXTRA\r\n\r\n", 200, "Body longer than expected"),
        (b"Content-Length: 0\r\n\r\n", 200, "Content-Length = 0"),
        (b"Content-Length: 70000000\r\n\r\n" + (b"a" * 70000000) + b"\r\n\r\n", 413, "Content-Length > MAX_CLIENT_SIZE"),
        (b"Transfer-Encoding: chunked\r\n\r\n5\r\nhello\r\n5\r\nworld\r\n0\r\n\r\n", 200, "Valid chunked body"),
        (b"Transfer-Encoding: chunked\r\n\r\n0\r\n\r\n", 200, "Chunked with size 0"),
        (b"Transfer-Encoding: chunked\r\n\r\nGGGG\r\nhello\r\n0\r\n\r\n", 400, "Chunked with invalid hex size"),
        (b"Transfer-Encoding: chunked\r\n\r\n5hello\r\n0\r\n\r\n", 400, "Chunked missing \\r\\n after size"),
        (b"Transfer-Encoding: chunked\r\n\r\n5\r\nhello0\r\n\r\n", 400, "Chunked missing \\r\\n after data"),
        (b"Transfer-Encoding: chunked\r\n\r\n5\r\nhello\r\n0\r\n", 408, "Chunked missing final \\r\\n"),
        (b"Transfer-Encoding: chunked\r\n\r\n10\r\nhello\r\n0\r\n\r\n", 408, "Chunked data shorter than announced"),
        (b"Transfer-Encoding: chunked\r\n\r\n2\r\nhello\r\n0\r\n\r\n", 400, "Chunked data longer than announced"),
        (b"Transfer-Encoding: chunked\r\n\r\n-5\r\nhello\r\n0\r\n\r\n", 400, "Chunked with negative size"),
        (b"Transfer-Encoding: chunked\r\n\r\nFFFFFFFFFFFFFFFF\r\nhello\r\n0\r\n\r\n", 400, "Chunked with size overflow"),
        (b"Content-Length: 100\r\nTransfer-Encoding: chunked\r\n\r\n5\r\nhello\r\n0\r\n\r\n", 200, "Both Content-Length and chunked"),
        (b"Transfer-Encoding: chunked\r\n\r\n5;name=value\r\nhello\r\n0\r\n\r\n", 400, "Chunked with extensions"),
        (b"Transfer-Encoding: chunked\r\n\r\n5\r\nhello\r\n0\r\nX-trailer: value\r\n\r\n", 400, "Chunked with trailer headers"),
        (b"Transfer-Encoding: chunked\r\n\r\n1\r\na\r\n1\r\nb\r\n1\r\nc\r\n0\r\n\r\n", 200, "multiple chunks"),
        (b"Transfer-Encoding: chunked\r\n\r\n100000\r\n" + (b"a" * 100000) + b"\r\n0\r\n\r\n", 200, "Very large single chunk"),
        (b"Content-Length: 5\r\n\r\nhe\x00lo\r\n\r\n", 200, "Body with null bytes"),
        (b"\r\n", 200, "No Content-Length or chunked"),
        (b"Transfer-Encoding: chunked\r\n\r\n5\nhello\n0\n\n", 408, "Chunked with only \\n instead of \\r\\n"),
        (b"Content-Length: 67108864\r\n\r\n" + (b"a" * 67108864) + b"\r\n\r\n", 200, "Body exactly at MAX_CLIENT_SIZE"),
        (b"Content-Length: 67108865\r\n\r\n" + (b"a" * 67108865) + b"\r\n\r\n", 413, "Body exactly 1 byte over MAX_CLIENT_SIZE"),
        (b"Transfer-Encoding: chunked\r\n\r\n4000000\r\n" + (b"a" * 67108865) + b"\r\n1\r\nx\r\n0\r\n\r\n", 413, "Chunked accumulating to > MAX_CLIENT_SIZE"),
        (b"Transfer-Encoding: chunked\r\n\r\n5\r\nhello\r\n0\r\n\r\n5\r\nworld\r\n0\r\n\r\n", 400, "Empty chunk in the middle"),
        (b"Content-Length: 10\r\n\r\n\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\r\n\r\n", 200, "Binary data in body")
    ]
    
    print("\033[1mTesting body parsing:\033[0m")
    
    for test, expected_code, description in host_headers_to_try:
        time.sleep(0.1)
        request = (f"POST /cgi-bin/uploads HTTP/1.1\r\nHost: localhost:{PORT}\r\n".encode() + test)
        status_code = getRequestCode(request)
        status_str = str(status_code) if status_code else 'None'
        
        if status_code == expected_code:
            print(f"{description:40} : \033[32m{status_str:>3}\033[0m | expected: {expected_code:3} \033[32m✓\033[0m")
        else:
            print(f"{description:40} : \033[31m{status_str:>5}\033[0m | expected: {expected_code:3} \033[31m✗\033[0m")

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
        request = f"GET {path} HTTP/1.1\r\nHost: localhost:{PORT}\r\n\r\n".encode()
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
        code = getRequestCode(f"GET {u} HTTP/1.1\r\nHost: localhost:{PORT}\r\n\r\n".encode())

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
        (f"GET /index.html HTTP/1.1!!\r\nHost: localhost:{PORT}\r\n\r\n".encode, "Garbage characters in request line", 400),
        (f"GET / HTTP/1.1\r\nHost: local\0\r\1\2\n\n\n\nhost:{PORT}\r\n\r\n".encode, "Header with illegal characters", 400),
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
