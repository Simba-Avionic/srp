import json
import socket
import struct

from some_ip_header import SomeIPHeader


class DataSender:
    def __init__(self, receiver_host, receiver_port):
        self.receiver_host = receiver_host
        self.receiver_port = receiver_port
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    def send_data(self, value: int, service_id: int, method_id: int):
        hdr = SomeIPHeader(service_id, method_id)
        payload = struct.pack('H', value)
        msg = b''.join(hdr.get_some_ip_data_payload(payload))
        self.socket.sendto(msg, (self.receiver_host, self.receiver_port))
        print("DATA SENT")
