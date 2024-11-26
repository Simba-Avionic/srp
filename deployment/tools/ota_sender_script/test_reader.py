import socket
from utils.someip_header import SomeIPHeader
import logging
logging.basicConfig(level=logging.INFO)
with socket.socket(socket.AF_INET,socket.SOCK_DGRAM) as sock:
    file=open("new_tar.tar",'wb')
    logging.info("Starting receiver thread")
    sock.bind(("127.0.0.1",10000))
    while True:
        data=sock.recv(4000)
        if len(data)>0:
            hdr=SomeIPHeader(0x0000,0x0000)
            hdr.read(data)
            if (hdr.service_id!=0x0100):
                logging.error(f"invalid service id: {hdr.service_id}")
                pass
            if(hdr.methode_id!=0x0000):
                logging.error(f"invalid method id: {hdr.methode_id}")
                pass
            formatted_crc = ''.join([f'\\0x{byte:02X}' for byte in data[-2:]])
            formatted_data_id= ''.join([f'\\0x{byte:02X}' for byte in data[16:18]])
            formatted_data=''.join([f'\\0x{byte:02X}' for byte in data[18:-2]])
            logging.info(f"""
    service_id:{hdr.service_id},
    method_id:{hdr.methode_id},
    lenght:{hdr.length},
    data_id:{formatted_data_id},
    crc:{formatted_crc},
    data:{formatted_data}""")
            file.write(data[18:-2])