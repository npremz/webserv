#!/usr/bin/python3

import sys
import os

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
    fields = []
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
        name_match = re.search(rb'name="([^"]+)"', disposition_line)
        filename_match = re.search(rb'filename="([^"]+)"', disposition_line)
        name = name_match.group(1).decode() if name_match else None
        filename = filename_match.group(1).decode() if filename_match else None
        if content.endswith(b'\r\n'):
            content = content[:-2]
        fields.append((name, filename, content))
    return fields

def save_uploaded_files(fields_list, prefix, upload_dir="uploads", start_index=0):
    if not os.path.exists(upload_dir):
        os.makedirs(upload_dir)
    files_saved = []
    for i in range(start_index, len(fields_list)):
        name, filename, content = fields_list[i]
        if filename:  # Ce champ est un fichier
            safe_filename = os.path.basename(filename)
            new_filename = f"{prefix}_{safe_filename}"
            filepath = os.path.join(upload_dir, new_filename)
            with open(filepath, 'wb') as f:
                f.write(content)
            files_saved.append(filepath)
    return files_saved

def main():
    try:
        if os.environ.get('REQUEST_METHOD') != "POST":
            response = create_http_error(405)
            print(response)
            return
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
        fields_list = parse_multipart_fields(post_body, boundary)
        
        prefix_value = None
        prefix_pos = None
        for i, (name, filename, content) in enumerate(fields_list):
            if name == "prefix":
                prefix_value = content.decode(errors='replace').strip()
                prefix_pos = i
                break
        if prefix_value is None or prefix_value == "":
            response = create_http_error(400, "Prefix value missing.")
            print(response)
            return
        
        files_saved = save_uploaded_files(fields_list, prefix_value, upload_path, start_index=prefix_pos+1)
        response = create_http_response(f"Saved files: {files_saved}", 200)
    except Exception as e:
        response = create_http_error(500, str(e))
    print(response)

if __name__ == '__main__':
    main()
