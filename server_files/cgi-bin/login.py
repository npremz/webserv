#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
import os
import time
import json
import base64
from urllib.parse import parse_qs

USERS_FILE = "users.txt"

# Sessions
SESSIONS_DIR = "sessions"
SESSION_COOKIE = "SESSIONID"
SESSION_TTL = 3600  # secondes, 1h

def ensure_sessions_dir():
    try:
        if not os.path.isdir(SESSIONS_DIR):
            os.makedirs(SESSIONS_DIR, 0o700)
    except Exception:
        pass

def now():
    return int(time.time())

def new_expiry(ttl=SESSION_TTL):
    return now() + int(ttl)

def generate_session_id(nbytes=32):
    raw = os.urandom(nbytes)
    sid = base64.urlsafe_b64encode(raw).decode('ascii').rstrip("=")
    return sid

def session_path(session_id):
    safe = "".join(c for c in session_id if c.isalnum() or c in "-_")
    return os.path.join(SESSIONS_DIR, safe)

def save_session(session_id, login, expires_at):
    ensure_sessions_dir()
    path = session_path(session_id)
    data = {"login": login, "expires_at": int(expires_at)}
    tmp = path + ".tmp"
    with open(tmp, "w") as f:
        json.dump(data, f)
        f.flush()
        os.fsync(f.fileno())
    os.rename(tmp, path)

def load_session(session_id):
    try:
        path = session_path(session_id)
        with open(path, "r") as f:
            data = json.load(f)
        return data
    except Exception:
        return None

def destroy_session(session_id):
    try:
        path = session_path(session_id)
        if os.path.exists(path):
            os.remove(path)
    except Exception:
        pass

def refresh_session(session_id, ttl=SESSION_TTL):
    data = load_session(session_id)
    if not data:
        return False
    data["expires_at"] = new_expiry(ttl)
    path = session_path(session_id)
    tmp = path + ".tmp"
    with open(tmp, "w") as f:
        json.dump(data, f)
        f.flush()
        os.fsync(f.fileno())
    os.rename(tmp, path)
    return True

def parse_query_string():
    qs = os.environ.get("QUERY_STRING", "")
    return parse_qs(qs)

def build_set_cookie(name, value, max_age=SESSION_TTL, path="/", http_only=True, same_site="Lax"):
    attrs = ["%s=%s" % (name, value)]
    if max_age is not None:
        attrs.append("Max-Age=%d" % int(max_age))
    if path:
        attrs.append("Path=%s" % path)
    if os.environ.get("HTTPS", "").lower() in ("on", "1"):
        attrs.append("Secure")
    if http_only:
        attrs.append("HttpOnly")
    if same_site:
        attrs.append("SameSite=%s" % same_site)
    return "; ".join(attrs)

def clear_cookie(name, path="/"):
    return build_set_cookie(name, "deleted", max_age=0, path=path)

def parse_cookies():
    raw = os.environ.get("HTTP_COOKIE", "")
    cookies = {}
    if not raw:
        return cookies
    parts = [p.strip() for p in raw.split(";")]
    for p in parts:
        if "=" in p:
            k, v = p.split("=", 1)
            cookies[k.strip()] = v.strip()
    return cookies

def create_http_response(body, status_code=200, content_type="text/plain; charset=utf-8", extra_headers=None):
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

    body_bytes = body.encode("utf-8")
    headers = [
        "Status: %d"% (status_code),
        "Content-Type: %s" % content_type,
        "Content-Length: %d" % len(body_bytes),
    ]
    if extra_headers:
        for k, v in extra_headers:
            headers.append("%s: %s" % (k, v))
    response = "\r\n".join(headers) + "\r\n\r\n" + body
    return response

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
    body = "%d %s\n%s" % (status_code, reason, message) if message else "%d %s\n" % (status_code, reason)
    return create_http_response(body, status_code=status_code, content_type="text/plain; charset=utf-8")

def authentification(login, password):
    try:
        with open(USERS_FILE, "r") as file:
            for line in file:
                if ":" not in line:
                    continue
                l, p = line.strip().split(":", 1)
                if l == login:
                    if p == password:
                        return "logged"
                    else:
                        return "wrong pass"
    except FileNotFoundError:
        return "server failed"
    return "no user"

def already_logged_response(login, session_id, refresh=True):
    headers = []
    if refresh:
        if refresh_session(session_id, SESSION_TTL):
            headers.append(("Set-Cookie", build_set_cookie(SESSION_COOKIE, session_id, max_age=SESSION_TTL)))
    body = "Déjà connecté en tant que %s.\n" % login
    return create_http_response(body, extra_headers=headers)

def handle_logout(session_id, reason="Logout requested"):
    destroy_session(session_id)
    headers = [("Set-Cookie", clear_cookie(SESSION_COOKIE))]
    return create_http_response("Déconnecté. (%s)\n" % reason, status_code=200, extra_headers=headers)

def create_json_response(obj, status_code=200, extra_headers=None):
    import json
    body = json.dumps(obj)
    return create_http_response(body, status_code=status_code,
                                content_type="application/json; charset=utf-8",
                                extra_headers=extra_headers)

def accept_json():
    qs = parse_query_string()
    if qs.get("format", [""])[0].lower() == "json":
        return True
    accept = os.environ.get("HTTP_ACCEPT", "")
    return "application/json" in accept

def main():
    try:
        cookies = parse_cookies()
        session_id = cookies.get(SESSION_COOKIE)
        session = None

        if session_id:
            session = load_session(session_id)
            if not session or int(session.get("expires_at", 0)) < now():
                if session_id:
                    destroy_session(session_id)
                session = None
                session_id = None

        qs = parse_query_string()


        content_length = int(os.environ.get('CONTENT_LENGTH', 0))
        if (content_length <= 0 and os.environ.get('REQUEST_METHOD') == "POST"):
            print(create_http_error(400, "This route requires a body on POST method")); return
        post_body = sys.stdin.read(content_length)

        if qs.get("action", [""])[0].lower() == "logout":
            resp = handle_logout(session_id or "", reason="GET logout")
            print(resp); return
        if os.environ.get('REQUEST_METHOD') == "POST":
            content_type = os.environ.get('CONTENT_TYPE', '')
            if "application/x-www-form-urlencoded" in content_type:
                
                data = parse_qs(post_body)
                login = data.get('login', [''])[0]
                password = data.get('password', [''])[0]
                action = data.get('action', [''])[0]
                if action.lower() == "logout":
                    resp = handle_logout(session_id or "", reason="POST logout")
                    print(resp); return
            else:
                login = password = action = ""

        if os.environ.get('REQUEST_METHOD') == "GET":
            if accept_json():
                if session:
                    refresh_session(session_id, SESSION_TTL)
                    set_cookie = build_set_cookie(SESSION_COOKIE, session_id, max_age=SESSION_TTL)
                    resp = create_json_response({"loggedIn": True, "login": session.get("login", "")},
                                                extra_headers=[("Set-Cookie", set_cookie)])
                else:
                    resp = create_json_response({"loggedIn": False})
                print(resp); return
            else:
                if session:
                    print(already_logged_response(session.get("login",""), session_id, refresh=True)); return
                else:
                    print(create_http_response("Not logged in.\n")); return

        if os.environ.get('REQUEST_METHOD') != "POST":
            print(create_http_error(405)); return
        content_type = os.environ.get('CONTENT_TYPE', '')
        if "application/x-www-form-urlencoded" not in content_type:
            print(create_http_error(415)); return

        data = parse_qs(post_body)
        login = data.get('login', [''])[0]
        password = data.get('password', [''])[0]
        action = data.get('action', [''])[0]

        if not login or not password:
            print(create_http_error(400, "Missing login or password in request.")); return

        log_status = authentification(login, password)
        if log_status == "logged":
            if session_id and not session:
                destroy_session(session_id)
            new_sid = generate_session_id()
            save_session(new_sid, login, new_expiry(SESSION_TTL))
            set_cookie = build_set_cookie(SESSION_COOKIE, new_sid, max_age=SESSION_TTL)
            if accept_json():
                print(create_json_response({"loggedIn": True, "login": login},
                                           extra_headers=[("Set-Cookie", set_cookie)])); return
            else:
                print(create_http_response("Logged successfully as %s.\n" % login,
                                           extra_headers=[("Set-Cookie", set_cookie)])); return
        elif log_status == "wrong pass":
            print(create_http_error(401, "Invalid password.")); return
        elif log_status == "no user":
            print(create_http_error(404, "User not found.")); return
        else:
            print(create_http_error(500, "Cannot consult existing users.")); return

    except Exception as e:
        print(create_http_error(500, e.__str__()))


if __name__ == '__main__':
    main()
