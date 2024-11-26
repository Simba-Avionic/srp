

class CrcGenerator:
    """Generator 16-bit CCITT-FALSE CRC16"""
    def __init__(self):
        pass

    def crc16_ccitt_false(self,data):
        "return 16-bit CCITT-FALSE CRC16"
        crc = 0xFFFF

        for byte in data:
            crc ^= byte << 8
            for _ in range(8):
                if crc & 0x8000:
                    crc = (crc << 1) ^ 0x1021
                else:
                    crc <<= 1

        crc ^= 0x0000
        return crc & 0xFFFF