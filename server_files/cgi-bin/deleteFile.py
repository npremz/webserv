#!/usr/bin/python3
# -*- coding: utf-8 -*-

import sys
import os
from urllib.parse import parse_qs

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

    if isinstance(body, str):
        body_bytes = body.encode("utf-8")
        content_length = len(body_bytes)
        payload = body
    else:
        content_length = len(body)
        payload = body.decode("utf-8", errors="replace")

    response_lines = [
        f"HTTP/1.1 {status_code} {reason}",
        f"Content-Type: {content_type}",
        f"Content-Length: {content_length}",
        "Connection: close",
        "",
        payload
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
    return create_http_response(body, status_code=status_code, content_type="text/plain; charset=utf-8")

def safe_delete_file(relative_path, base_dir):

    if not relative_path:
        return False, 400, "Paramètre 'file' manquant."

    if os.path.isabs(relative_path):
        return False, 400, "Chemin absolu interdit."
    rel_clean = relative_path.lstrip("/").strip()
    if not rel_clean:
        return False, 400, "Chemin vide après normalisation."

    base_real = os.path.realpath(base_dir)
    target_real = os.path.realpath(os.path.join(base_real, rel_clean))

    if not (target_real == base_real or target_real.startswith(base_real + os.sep)):
        return False, 403, "Accès hors du répertoire autorisé."

    if not os.path.exists("." + target_real):
        return False, 404, "Fichier introuvable."
    if not os.path.isfile("." + target_real):
        return False, 415, "Le chemin indiqué n'est pas un fichier."

    try:
        os.remove("." + target_real)
        return True, 204, "" 
    except PermissionError:
        return False, 403, "Permissions insuffisantes pour supprimer ce fichier."
    except FileNotFoundError:
        return False, 404, "Fichier introuvable (peut avoir déjà été supprimé)."
    except OSError as e:
        return False, 500, f"Erreur système: {e}"

def main():
    try:
        if os.environ.get("REQUEST_METHOD") != "DELETE":
            print(create_http_error(405, "Seule la méthode DELETE est autorisée."))
            return

        upload_path = os.environ.get("UPLOAD_PATH")
        document_root = os.environ.get("DOCUMENT_ROOT")

        if not upload_path:
            print(create_http_error(500, "UPLOAD_PATH non défini sur le serveur."))
            return
        if not document_root:
            print(create_http_error(500, "DOCUMENT_ROOT non défini sur le serveur."))
            return

        base_dir = os.path.join(document_root, upload_path)

        qs = os.environ.get("QUERY_STRING", "")
        params = parse_qs(qs, keep_blank_values=True)
        target_rel = params.get("file", [""])[0]

        ok, code, msg = safe_delete_file(target_rel, base_dir)
        if ok:
            response = create_http_response("", status_code=204, content_type="text/plain")
        else:
            response = create_http_error(code, msg)

        print(response)
    except Exception as e:
        print(create_http_error(500, str(e)))

if __name__ == "__main__":
    main()
