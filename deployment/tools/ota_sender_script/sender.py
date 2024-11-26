from file_reader import FileReader
from crc_generator import CrcGenerator
from someip_header import SomeIPHeader
import socket
import threading
import time

class Sender:
    def __init__(self,filename:str,callback):
        self._reader=FileReader()
        self._crc_generator=CrcGenerator()
        self._data:list(list)=self._reader.read_data_in_parts(filename)
        self._udp_sock=socket.socket(socket.AF_INET,socket.SOCK_DGRAM)
        self._Rudp_sock=socket.socket(socket.AF_INET,socket.SOCK_DGRAM)
        self.transmition_num=1
        self._callback=callback

    def sendData(self,ip:str,port:int):
        """ip adress to send data,port """
        hdr=SomeIPHeader(0x0000,0x0000,0x00) # JESLI TRZEBA ZMIEN SERVICE_ID,METHOD_ID,MSG_TYPE
        for part in self._data:
            crc=self._crc_generator.crc16_ccitt_false(part)
            part=self.transmition_num.to_bytes(2,byteorder='big')+part+crc.to_bytes(2,byteorder='big')
            to_send_data=hdr.GetSomeIPDataPayload(part)
            self._udp_sock.sendto(b''.join(to_send_data),(ip,port))

    def StartReceiverThread(self):
        threading.Thread(target=self._ReceiverThread,daemon=True).start()
        time.sleep(2)

    def _ReceiverThread(self):
        try:
            self._Rudp_sock.bind(("127.0.0.1",1000))
            while True:
                data,client_adress=self._Rudp_sock.recvfrom(2400)
                self._callback(data,client_adress)
        finally:
            self._Rudp_sock.close()


