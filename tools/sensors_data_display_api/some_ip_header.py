from ctypes import sizeof, c_uint16, c_uint8, c_uint32, c_uint64


class SomeIPHeader:
    def __str__(self) -> str:
        return f"""
                service_id:{self.service_id}
                method_id:{self.methode_id}
                lenght:{self.length}
                msg_type:{self.msg_typ}
                payload:{self.raw_payload}"""

    def __init__(self, service_id=None, methode_id=None, msg_typ=0x01) -> None:
        self.raw_payload = None
        self.service_id = service_id
        self.methode_id = methode_id
        self.payload = []
        self.length = 0x0008
        self.client_id = 0x0001
        self.session = 0x0001
        self.msg_typ = msg_typ
        self.msg_return = 0x00

    def read(self, hex_data) -> None:
        self.service_id = int.from_bytes(hex_data[0:2], byteorder="big", signed=False)
        self.methode_id = int.from_bytes(hex_data[2:4], byteorder="big", signed=False)
        self.length = int.from_bytes(hex_data[4:8], byteorder="big", signed=False)
        self.msg_typ = hex_data[14]
        self.raw_payload = hex_data[16:]

    def set_payload(self, payload) -> None:
        self.payload = payload
        self.length = 0x8 + len(payload)

    def get_some_ip_data(self):
        res = []
        temp_res = []
        size = 0
        for item in self.payload:
            if (item <= 255):
                size = size + 1
                temp_res.append(item.to_bytes(sizeof(c_uint8), byteorder="little"))
            else:
                size = size + 2
                temp_res.append(item.to_bytes(sizeof(c_uint16), byteorder="little"))
        self.length = 0x8 + size
        res.append(self.service_id.to_bytes(sizeof(c_uint16), byteorder="big"))
        res.append(self.methode_id.to_bytes(sizeof(c_uint16), byteorder="big"))
        res.append(self.length.to_bytes(sizeof(c_uint32), byteorder="big"))
        res.append(self.client_id.to_bytes(sizeof(c_uint16), byteorder="big"))
        res.append(self.session.to_bytes(sizeof(c_uint16), byteorder="big"))
        res.append(0x01.to_bytes(sizeof(c_uint8), byteorder="big"))
        res.append(0x01.to_bytes(sizeof(c_uint8), byteorder="big"))
        res.append(self.msg_typ.to_bytes(sizeof(c_uint8), byteorder="big"))
        res.append(self.msg_return.to_bytes(sizeof(c_uint8), byteorder="big"))
        for item in temp_res:
            res.append(item)

        return res

    def get_some_ip_data_payload(self, payload):
        res = []
        self.length = 0x8 + len(payload)
        res.append(self.service_id.to_bytes(sizeof(c_uint16), byteorder="big"))
        res.append(self.methode_id.to_bytes(sizeof(c_uint16), byteorder="big"))
        res.append(self.length.to_bytes(sizeof(c_uint32), byteorder="big"))
        res.append(self.client_id.to_bytes(sizeof(c_uint16), byteorder="big"))
        res.append(self.session.to_bytes(sizeof(c_uint16), byteorder="big"))
        res.append(0x01.to_bytes(sizeof(c_uint8), byteorder="big"))
        res.append(0x01.to_bytes(sizeof(c_uint8), byteorder="big"))
        res.append(self.msg_typ.to_bytes(sizeof(c_uint8), byteorder="big"))
        res.append(self.msg_return.to_bytes(sizeof(c_uint8), byteorder="big"))
        res.append(payload)
        return res
