import socket
import struct
import random
import time
import ctypes

from some_ip_header import SomeIPHeader


first = True
HOST = "127.0.0.1"
PORT = 10101

with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as sock:
    sock.connect((HOST, PORT))
    i = 0
    while True:
        i += 1
        # temperature up
        hdr = SomeIPHeader(514, 32769)
        payload = struct.pack('H',int(random.uniform(20, 25)*10))
        print(payload)
        msg = b''.join(hdr.get_some_ip_data_payload(payload))
        sock.sendto(msg, (HOST, PORT))
        # temperature down
        hdr = SomeIPHeader(514, 32770)
        payload = struct.pack('H',int(random.uniform(10, 15)*10))
        msg = b''.join(hdr.get_some_ip_data_payload(payload))
        sock.sendto(msg, (HOST, PORT))
        # temperature middle
        hdr = SomeIPHeader(514, 32771)
        payload = struct.pack('H',int(random.uniform(0, 5)*10))
        msg = b''.join(hdr.get_some_ip_data_payload(payload))
        sock.sendto(msg, (HOST, PORT))
        # tank pressure
        hdr = SomeIPHeader(514, 32773)
        payload = struct.pack('f', random.uniform(1, 3.3))
        msg = b''.join(hdr.get_some_ip_data_payload(payload))
        sock.sendto(msg, (HOST, PORT))
        # main valve
        hdr = SomeIPHeader(515, 32769)
        payload = f"test{i}".encode("UTF-8")
        msg = b''.join(hdr.get_some_ip_data_payload(payload))
        sock.sendto(msg, (HOST, PORT))
        # pressure difference
        hdr = SomeIPHeader(514, 32774)
        payload = struct.pack('f', random.uniform(20, 25))
        msg = b''.join(hdr.get_some_ip_data_payload(payload))
        sock.sendto(msg, (HOST, PORT))

        time.sleep(0.1)
