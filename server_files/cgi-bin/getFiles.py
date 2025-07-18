#!/usr/bin/python3

import sys
import os
import json

def create_http_response(body, status_code=200, content_type="application/json"):
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
    body = json.dumps({
        "error": {
            "code": status_code,
            "message": reason + (": " + message if message else "")
        }
    })
    return create_http_response(body, status_code=status_code, content_type="application/json")

def list_uploads():
    uploads_dir = "uploads"
    if not os.path.isdir(uploads_dir):
        return []
    files = [f for f in os.listdir(uploads_dir) if not f.startswith('.') and os.path.isfile(os.path.join(uploads_dir, f))]
    return files

def main():
    try:
        if os.environ.get('REQUEST_METHOD') != "GET":
            response = create_http_error(405)
            print(response)
            return
        files = list_uploads()
        result = {
            "count": len(files),
            "files": files
        }
        body = json.dumps(result)
        response = create_http_response(body)
    except Exception as e:
        response = create_http_error(500, e.__str__())
    print(response)

if __name__ == '__main__':
    main()
