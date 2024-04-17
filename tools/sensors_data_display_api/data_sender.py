import socket
import struct

from some_ip_header import SomeIPHeader


class DataSender:
    def __init__(self):
        self.receiver_host = '127.0.0.1'
        self.receiver_port = 10101
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.socket.connect((self.receiver_host, self.receiver_port))

    def send_data(self, value: int, service_id: int, method_id: int):
        hdr = SomeIPHeader(service_id, method_id)
        payload = struct.pack('H', value)
        msg = b''.join(hdr.get_some_ip_data_payload(payload))
        self.socket.sendto(msg, (self.receiver_host, self.receiver_port))
        print("DATA SENT")
