from utils.file_reader import FileReader
from utils.crc_generator import CrcGenerator
from utils.someip_header import SomeIPHeader
import socket
import threading
import time
import ctypes
import logging

logging.basicConfig(level=logging.WARNING)

class Sender:
    def __init__(self,filename:str,callback):
        self._reader=FileReader()
        self._crc_generator=CrcGenerator()
        self.filename=filename
        self.dictData=self.__generateDataDict()
        self._udp_sock=socket.socket(socket.AF_INET,socket.SOCK_DGRAM)
        self._Rudp_sock=socket.socket(socket.AF_INET,socket.SOCK_DGRAM)
        self.last_transmition=0
        self._callback=callback
        self.ok_transmision=[]
        self.receiver_ip="127.0.0.1"
        self.receiver_port=10000

    def __generateDataDict(self):
        dictData={}
        i=1
        for x in self._reader.read_data_in_parts(self.filename):
            dictData[i]=x
            i+=1
        self.last_transmition=i
        print(i)
        return dictData

    def sendData(self):
        """ip adress to send data,port """
        hdr=SomeIPHeader(0x0100,0x0000,0x00) # JESLI TRZEBA ZMIEN SERVICE_ID,METHOD_ID,MSG_TYPE
        file=open("abc.tar","wb")
        to_save=[]
        for key,value in self.dictData.items():
            """ struktura payload
                | data_ID (key) 2B |
                | dane (value) max 251B |
                | crc wyliczone i dodane przez someip 2B |"""
            payload=key.to_bytes(ctypes.sizeof(ctypes.c_uint16),byteorder='big')
            payload+=value
            data=hdr.GetSomeIPDataPayload(payload)
            self._udp_sock.sendto(data,(self.receiver_ip,self.receiver_port))
            to_save.append(data[18:-2])
            logging.debug("data send succesfull")
        data=hdr.GetSomeIPDataPayload(int.to_bytes(self.last_transmition,byteorder="big",length=2,signed=False),False)
        self._udp_sock.sendto(data,(self.receiver_ip,self.receiver_port))


