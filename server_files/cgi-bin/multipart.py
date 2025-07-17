#!/usr/bin/python3

import sys
import os
from urllib.parse import parse_qs

USERS_FILE = "./server_files/cgi-bin/users.txt"

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
        415: "Unsupported Media Type",
        500: "Internal Server Error",
        502: "Bad Gateway",
        503: "Service Unavailable"
    }.get(status_code, "Error")
    body = f"{status_code} {reason}\n{message}" if message else f"{status_code} {reason}\n"
    return create_http_response(body, status_code=status_code, content_type="text/plain")

def extract_boundary(content_type):
    if not content_type:
        return None
    parts = content_type.split(';')
    for part in parts:
        part = part.strip()
        if part.startswith('boundary='):
            boundary = part[9:]
            if boundary.startswith('"') and boundary.endswith('"'):
                boundary = boundary[1:-1]
            return boundary
    return None

def parse_multipart_fields(post_body, boundary):
    fields = {}
    boundary_marker = b'--' + boundary.encode()
    parts = post_body.split(boundary_marker)
    for part in parts:
        part = part.strip(b'\r\n')
        if not part or part == b'--':
            continue
        try:
            header_part, content = part.split(b'\r\n\r\n', 1)
        except ValueError:
            continue
        headers = header_part.split(b'\r\n')
        disposition_line = None
        for h in headers:
            if h.lower().startswith(b'content-disposition:'):
                disposition_line = h
                break
        if not disposition_line:
            continue

        import re
        filename_match = re.search(rb'filename="([^"]+)"', disposition_line)
        name_match = re.search(rb'name="([^"]+)"', disposition_line)
        filename = filename_match.group(1).decode() if filename_match else None
        name = name_match.group(1).decode() if name_match else None

        if content.endswith(b'\r\n'):
            content = content[:-2]
        fields[name] = (filename, content)
    return fields

def save_uploaded_files(fields, prefix, upload_dir="./server_files/uploads"):
    if not os.path.exists(upload_dir):
        os.makedirs(upload_dir)
    files_saved = []
    for name in fields:
        filename, content = fields[name]
        if filename is not None:
            safe_filename = os.path.basename(filename)
            new_filename = f"{prefix}_{safe_filename}"
            filepath = os.path.join(upload_dir, new_filename)
            with open(filepath, 'wb') as f:
                f.write(content)
            files_saved.append(filepath)
    return files_saved

def main():
    try:
        if "multipart/form-data" not in os.environ.get('CONTENT_TYPE'):
            response = create_http_error(415)
            print(response)
            return
        content_length = int(os.environ.get('CONTENT_LENGTH', 0))
        uploads_allowed = os.environ.get("UPLOAD_ENABLE")
        if uploads_allowed != "on":
            response = create_http_error(403, )
            print(response)
            return
        upload_path = os.environ.get("UPLOAD_PATH")
        if len(upload_path) == 0:
            response = create_http_error(500, "upload_path not defined on the server")
            print(response)
            return
        upload_path = os.environ.get("DOCUMENT_ROOT") + upload_path

        post_body = sys.stdin.buffer.read(content_length)
        boundary = extract_boundary(os.environ.get('CONTENT_TYPE'))
        fields = parse_multipart_fields(post_body, boundary)
        prefix_info = fields.get("prefix")
        if not prefix_info or not prefix_info[1].strip():
            response = create_http_error(400, "Prefix value missing.")
            print(response)
            return
        
        prefix_value = prefix_info[1].decode(errors='replace').strip()
        saved = save_uploaded_files(fields, prefix_value, upload_path)
        response = create_http_response(f"Saved files: {saved}", 200)
    except Exception as e:
        response = create_http_error(500, str(e))
    print(response)

if __name__ == '__main__':
    main()
