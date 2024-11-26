from utils.sender import Sender
import time
import logging

logging.basicConfig(level=logging.DEBUG)

def socketCallback(data,client_adress):
    print(data)
    print(client_adress)

s=Sender("test_tar.tar",socketCallback)
s.sendData("127.0.0.1",1000) # TU PODMIENIASZ NA ADRES SWOJEGO SERWISU
time.sleep(10)

