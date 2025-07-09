#!/usr/bin/python3

import sys
import os

def main():
    # Récupère la taille du body envoyée par le client (Content-Length)
    content_length = int(os.environ.get('CONTENT_LENGTH', 0))
    post_body = sys.stdin.read(content_length)

    # Écrit la réponse HTTP complète avec le body
    print("HTTP/1.1 200 OK\r")
    print("Content-Type: text/plain\r")
    print(f"Content-Length: {len(post_body)}\r")
    print("\r")
    print(post_body)

if __name__ == '__main__':
    main()
