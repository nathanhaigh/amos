#!/usr/bin/python
import sys
import socket
import traceback

####

def build_exploit():
    req =   "aaaa\n"
    return req

####

def send_req(host, port, req):
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    print("Connecting to %s:%d..." % (host, port))
    sock.connect((host, port))

    print("Connected, sending request...")
    sock.send(req)

    print("Request sent, waiting for reply...")
    rbuf = sock.recv(1024)
    resp = ""
    while len(rbuf):
	resp = resp + rbuf
	rbuf = sock.recv(1024)

    print("Received reply.")
    sock.close()
    return resp

####

if len(sys.argv) != 3:
    print("Usage: " + sys.argv[0] + " host port")
    exit()

try:
    
    req = build_exploit()
    print("HTTP request:")
    print(req)

    resp = send_req(sys.argv[1], int(sys.argv[2]), req)
    print("HTTP response:")
    print(resp)
except:
    print("Exception:")
    print(traceback.format_exc())

