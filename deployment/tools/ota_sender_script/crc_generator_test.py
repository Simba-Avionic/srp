import unittest
from crc_generator import CrcGenerator

class CrcGeneratorTest(unittest.TestCase):
    def test_crc_generator(self):
        generator=CrcGenerator()
        l=[
            [[0x00,0x00,0x00,0x00],0x84C0],
            [[0xF2,0x01,0x83],0xD374],
            [[0xFF,0xFF,0xFF,0xFF],0x1D0F],
            [[0x33,0x22,0x55,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF],0xF53F]
        ]
        for data,crc in l:
            self.assertEqual(generator.crc16_ccitt_false(data),crc)


unittest.main()