import datetime
import binascii

from tools.uds.gui.singleton_lib import SingletonMeta
from tools.uds.gui.console import Console
from doipclient import DoIPClient

class UdsCon(metaclass=SingletonMeta):
    def __init__(self):
        self.client = None
    def AddCallback(self,onConnect,onDisconnect):
        self.onConnect = onConnect
        self.onDisconnect = onDisconnect
    def ConnectToEcu(self, ecu_ip):
        Console().AddLog(f"Connecting to {ecu_ip}")
        if self.client is not None:
            print("error")
            return
        try:
            self.client = DoIPClient(ecu_ip,0x0A)
            Console().AddLog(f"ECU connection successful on ip: {ecu_ip}")
            self.onConnect()
        except:
            Console().AddLog(f"Failed to establish a connection with the ECU on ip: {ecu_ip}")
    def Disconnect(self,Flag=True):
        self.client = None
        if Flag:
            Console().AddLog("Disconnected from ECU")
            self.onDisconnect()

    def SendCmd(self,data,rx_loop = None):
        try:
            Console().AddLog(f"tx: 0x{data}")
            self.client.send_diagnostic(bytearray.fromhex(data))
            self.receive_loop(rx_loop)
        except:
            Console().AddLog("Error")
            self.Disconnect()

    def receive_loop(self,rx_loop = None):
        try:
            res = bytearray(self.client.receive_diagnostic())
            Console().AddLog("Rx: 0x"+binascii.hexlify(res).decode("utf-8"))
            if(res[0] == 0x7f and res[2] == 0x78):
                self.receive_loop(rx_loop)
            else:
                if rx_loop is not None:
                    rx_loop(res)

        except:
            Console().AddLog("Error")
            self.Disconnect()