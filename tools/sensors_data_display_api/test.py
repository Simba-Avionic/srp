import socket
import struct
import random
import time

from some_ip_header import SomeIPHeader


first = True
HOST = "127.0.0.1"
PORT = 10101

with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as sock:
    while True:
        hdr = SomeIPHeader(0x01, 0x8002)
        payload = struct.pack('f', random.uniform(20, 25))
        msg = b''.join(hdr.get_some_ip_data_payload(payload))
        sock.sendto(msg, (HOST, PORT))
        hdr = SomeIPHeader(0x01, 0x8001)
        payload = struct.pack('f', random.uniform(0, 15))
        msg = b''.join(hdr.get_some_ip_data_payload(payload))
        sock.sendto(msg, (HOST, PORT))
        hdr = SomeIPHeader(0x01, 0x8003)
        payload = struct.pack('f', random.uniform(0, 3.3))
        msg = b''.join(hdr.get_some_ip_data_payload(payload))
        sock.sendto(msg, (HOST, PORT))
        hdr = SomeIPHeader(0x01, 0x8004)
        payload = struct.pack('f', random.uniform(0, 3.3))
        msg = b''.join(hdr.get_some_ip_data_payload(payload))
        sock.sendto(msg, (HOST, PORT))
        hdr = SomeIPHeader(0x01, 0x8005)
        payload = struct.pack('f', random.uniform(0, 3.3))
        msg = b''.join(hdr.get_some_ip_data_payload(payload))
        sock.sendto(msg, (HOST, PORT))
        hdr = SomeIPHeader(0x01, 0x8006)
        payload = struct.pack('f', random.uniform(0, 10000))
        msg = b''.join(hdr.get_some_ip_data_payload(payload))
        sock.sendto(msg, (HOST, PORT))
        hdr = SomeIPHeader(0x01, 0x8007)
        payload = struct.pack('f', random.uniform(0, 20)) + "28-062018f00883".encode("UTF-8")
        msg = b''.join(hdr.get_some_ip_data_payload(payload))
        sock.sendto(msg, (HOST, PORT))
        payload = struct.pack('f', random.uniform(0, 20)) + "28-3c29e381356d".encode("UTF-8")
        msg = b''.join(hdr.get_some_ip_data_payload(payload))
        sock.sendto(msg, (HOST, PORT))
        payload = struct.pack('f', random.uniform(0, 20)) + ("28-3c01e076e761".encode("UTF-8"))
        msg = b''.join(hdr.get_some_ip_data_payload(payload))
        sock.sendto(msg, (HOST, PORT))
        time.sleep(0.1)
