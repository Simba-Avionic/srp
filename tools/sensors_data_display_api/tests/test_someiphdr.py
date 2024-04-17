import unittest
import sys
from pathlib import Path
import struct

# Dodanie katalogu nadrzędnego do ścieżki
sys.path.append(str(Path(__file__).resolve().parent.parent))

from some_ip_header import SomeIPHeader

class TestSomeIpHeader(unittest.TestCase):
    def test_constructor(self):
        hdr = SomeIPHeader(0x22,0x23,0x1)
        self.assertEqual(hdr.service_id,0x22)
        self.assertEqual(hdr.methode_id,0x23)
        self.assertEqual(hdr.msg_typ,0x1)
        self.assertEqual(hdr.payload,[])
        self.assertEqual(hdr.length,0x8)
        self.assertEqual(hdr.client_id,0x1)
        self.assertEqual(hdr.session,0x1)
        self.assertEqual(hdr.msg_return,0x0)
        self.assertEqual(str(hdr),"""
                service_id:34
                method_id:35
                lenght:8
                msg_type:1
                payload:None""")

    def test_set_payload(self):
        hdr = SomeIPHeader(0x01,0x01)
        payload = [0,1,2,3,4]
        hdr.set_payload(payload)
        self.assertEqual(hdr.payload,payload)
        self.assertEqual(hdr.length,0x8+len(payload))
    def test_read(self):
        hdr = SomeIPHeader()
        hex_data=b'\x00\x01\x00\x01\x00\x00\x00\x10\x00\x01\x00\x01\x01\x01\x01\x00\x12\x13'
        hdr.read(hex_data)
        self.assertEqual(hdr.service_id,0x01)
        self.assertEqual(hdr.methode_id,0x01)
        self.assertEqual(hdr.length,0x10)
        self.assertEqual(hdr.msg_typ,0x1)
        self.assertEqual(hdr.raw_payload,b"\x12\x13")
    def test_get_some_ip_data(self):
        data = b'\x00"\x00#\x00\x00\x00\x08\x00\x01\x00\x01\x01\x01\x01\x00'
        hdr = SomeIPHeader(0x22,0x23,0x1)
        self.assertEqual(b"".join(hdr.get_some_ip_data()),data)
    def test_get_some_ip_data2(self):
        data = b'\x00"\x00#\x00\x00\x00\n\x00\x01\x00\x01\x01\x01\x01\x00{\x00'
        payload = struct.pack('H',123)
        hdr = SomeIPHeader(0x22,0x23,0x1)
        hdr.set_payload(payload)
        self.assertEqual(b"".join(hdr.get_some_ip_data()),data)
    def test_get_some_ip_data_payload(self):
        data = b'\x00"\x00#\x00\x00\x00\n\x00\x01\x00\x01\x01\x01\x01\x00{\x00'
        payload = struct.pack('H',123)
        hdr = SomeIPHeader(0x22,0x23,0x1)
        hdr.set_payload(payload)
        self.assertEqual(b"".join(hdr.get_some_ip_data_payload(payload)),data)
