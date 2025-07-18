import sys
import os
from urllib.parse import parse_qs

USERS_FILE = "./server_files/cgi-bin/users.txt"

def parse_post_data(post_body):
    data = parse_qs(post_body)
    login = data.get('login', [''])[0]
    password = data.get('password', [''])[0]
    return login, password

def create_http_response(body, status_code=200, content_type="text/plain"):
    reason = {
        200: "OK",
        201: "Created",
        204: "No Content",
        301: "Moved Permanently",
        302: "Found",
        400: "Bad Request",
        401: "Unauthorized",
        403: "Forbidden",
        404: "Not Found",
        405: "Method Not Allowed",
        415: "Unsupported Media Type",
        500: "Internal Server Error",
        502: "Bad Gateway",
        503: "Service Unavailable"
    }.get(status_code, "OK")
    
    response_lines = [
        f"HTTP/1.1 {status_code} {reason}",
        f"Content-Type: {content_type}",
        f"Content-Length: {len(body)}",
        "",
        body
    ]
    return "\r\n".join(response_lines)

def create_http_error(status_code, message=""):
    reason = {
        400: "Bad Request",
        401: "Unauthorized",
        403: "Forbidden",
        404: "Not Found",
        405: "Method Not Allowed",
        415: "Unsupported Media Type",
        500: "Internal Server Error",
        502: "Bad Gateway",
        503: "Service Unavailable"
    }.get(status_code, "Error")
    body = f"{status_code} {reason}\n{message}" if message else f"{status_code} {reason}\n"
    return create_http_response(body, status_code=status_code, content_type="text/plain")

def authentification(login, password):
    try:
        with open(USERS_FILE, "r") as file:
            for line in file:
                l, p = line.strip().split(":")
                if l == login:
                    if p == password:
                        return "logged"
                    else:
                        return "wrong pass"
    except FileNotFoundError:
        return "server failed"
    return "no user"

def main():
    try:
        if os.environ.get('REQUEST_METHOD') != "POST":
            response = create_http_error(405)
            print(response)
            return
        if "application/x-www-form-urlencoded" not in os.environ.get('CONTENT_TYPE'):
            response = create_http_error(415)
            print(response)
            return
        content_length = int(os.environ.get('CONTENT_LENGTH', 0))
        post_body = sys.stdin.read(content_length)
        login, password = parse_post_data(post_body)
        if not login or not password:
            response = create_http_error(400, "Missing login or password in request.")
        else:
            log_status = authentification(login, password)
            if log_status == "logged":
                response = create_http_response("Logged successfully.")
            elif log_status == "wrong pass":
                response = create_http_error(401, "Invalid password.")
            elif log_status == "no user":
                response = create_http_error(404, "User not found.")
            else:
                response = create_http_error(500, "Cannot consult existing users.")
    except Exception as e:
        response = create_http_error(500, e.__str__())
    print(response)

if __name__ == '__main__':
    main()
