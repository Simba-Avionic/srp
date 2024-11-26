import ctypes
from utils.crc_generator import CrcGenerator

class SomeIPHeader:
    def __init__(self, service_id,methode_id,msg_typ=0x01) -> None:
        self.service_id = service_id
        self.methode_id = methode_id
        self.payload = []
        self.length  =0x0008
        self.client_id = 0x0001
        self.session = 0x0001
        self.msg_typ = msg_typ
        self.msg_return = 0x00
    def read(self,hex_value)->None:
        self.service_id = int.from_bytes(hex_value[0:2],byteorder="big",signed=False)
        self.methode_id = int.from_bytes(hex_value[2:4],byteorder="big",signed=False)
        self.length = int.from_bytes(hex_value[4:8],byteorder="big",signed=False)
        self.msg_typ = hex_value[14]
        
    def SetPayload(self,payload) -> None:
        self.payload = payload
        self.length = 0x8+len(payload)

    def GetSomeIPData(self):
        res = []
        temp_res = []
        size = 0
        for item in self.payload:
            if(item <= 255) :
                size = size+1
                temp_res.append(item.to_bytes(ctypes.sizeof(ctypes.c_uint8), byteorder="little"))
            else:
                size = size+2
                temp_res.append(item.to_bytes(ctypes.sizeof(ctypes.c_uint16), byteorder="little"))
        self.length = 0x8+size
        res.append(self.service_id.to_bytes(ctypes.sizeof(ctypes.c_uint16), byteorder="big"))
        res.append(self.methode_id.to_bytes(ctypes.sizeof(ctypes.c_uint16), byteorder="big"))
        res.append(self.length.to_bytes(ctypes.sizeof(ctypes.c_uint32), byteorder="big"))
        res.append(self.client_id.to_bytes(ctypes.sizeof(ctypes.c_uint16), byteorder="big"))
        res.append(self.session.to_bytes(ctypes.sizeof(ctypes.c_uint16), byteorder="big"))
        res.append(0x01.to_bytes(ctypes.sizeof(ctypes.c_uint8), byteorder="big"))
        res.append(0x01.to_bytes(ctypes.sizeof(ctypes.c_uint8), byteorder="big"))
        res.append(self.msg_typ.to_bytes(ctypes.sizeof(ctypes.c_uint8), byteorder="big"))
        res.append(self.msg_return.to_bytes(ctypes.sizeof(ctypes.c_uint8), byteorder="big"))
        for item in temp_res:
            res.append(item)

        return res

    def GetSomeIPDataPayload(self, payload,addCrc:bool=True):
        res = bytearray()
        crcGenerator=CrcGenerator()
        self.length=0x010+len(payload)
        crc = crcGenerator.crc16_ccitt_false(payload)
        res += self.service_id.to_bytes(ctypes.sizeof(ctypes.c_uint16), byteorder="big")
        res += self.methode_id.to_bytes(ctypes.sizeof(ctypes.c_uint16), byteorder="big")
        res += self.length.to_bytes(ctypes.sizeof(ctypes.c_uint32), byteorder="big")
        res += self.client_id.to_bytes(ctypes.sizeof(ctypes.c_uint16), byteorder="big")
        res += self.session.to_bytes(ctypes.sizeof(ctypes.c_uint16), byteorder="big")
        res += (0x01).to_bytes(ctypes.sizeof(ctypes.c_uint8), byteorder="big")
        res += (0x01).to_bytes(ctypes.sizeof(ctypes.c_uint8), byteorder="big")
        res += self.msg_typ.to_bytes(ctypes.sizeof(ctypes.c_uint8), byteorder="big")
        res += self.msg_return.to_bytes(ctypes.sizeof(ctypes.c_uint8), byteorder="big")
        res += payload
        if addCrc:
            res += crc.to_bytes(ctypes.sizeof(ctypes.c_uint16), byteorder="big")
        return res