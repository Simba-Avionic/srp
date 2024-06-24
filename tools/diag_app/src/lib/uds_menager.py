from doipclient import DoIPClient
import binascii
from tools.diag_app.src.lib.db import DB, ECU, ECUStatus,SingletonMeta
import threading
import time
import queue



class UdsClient:
    def receive_loop(self,client,callback):
        temp = {}
        temp["ecu_id"] = self.ecu_.diag_id
        
        res = bytearray(client.receive_diagnostic())
        if(res[0] == 0x7f and res[2] == 0x78):
            # print("    "+binascii.hexlify(res).decode("utf-8")+" -> Response pending ")
            temp["req"] = ""+binascii.hexlify(res).decode("utf-8")+" -> Response pending "
            callback(temp)
            self.receive_loop(client,callback)
        else:
            # print("    "+binascii.hexlify(res).decode("utf-8"))
            temp["req"] = ""+binascii.hexlify(res).decode("utf-8")
            callback(temp)
        
    def send(self,client,data,callback):
        client.send_diagnostic(bytearray.fromhex(data))
        self.receive_loop(client, callback)
    def __init__(self,ecu: ECU,db:DB) -> None:
        self.ecu_ = ecu
        self.db_ = db
        self.queue = queue.Queue()
    def Start(self):
        
        self.t = threading.Thread(target=self.SendingLoop,args=(self,self.queue,self.ecu_.ip,int(self.ecu_.diag_id)))
        self.t.start()
        
    def Stop(self):
        self.queue.put(None)
        self.t.join()
        
    def SendRequest(self,req, callback):
        temp = {}
        temp["req"] = req
        temp["callback"] = callback
        self.queue.put(temp)
        
    def SendingLoop(self,selfobj,q:queue.Queue,ip:str,id:int, *args):
        print("Starting thread ["+ip+"]")
        StopToken = False
        while not StopToken:
            try:
                print("Connectting to "+ip)
                client = DoIPClient(ip,id)
                for i in selfobj.db_.ecu_list:
                    if i.diag_id == selfobj.ecu_.diag_id:
                        i.ecu_status = ECUStatus.ONLINE
                while True:
                    data = q.get()
                    if data is None:
                        StopToken= True
                        break
                    selfobj.send(client,data["req"],data["callback"])
            except:
                for i in selfobj.db_.ecu_list:
                    if i.diag_id == selfobj.ecu_.diag_id:
                        i.ecu_status = ECUStatus.OFFLINE
                time.sleep(0.5)
        print("Stopping thread")

class UdsMenager(metaclass=SingletonMeta):
    def __init__(self, db:DB) -> None:
        self.db_ = db
        self.client_list = {}
    def AddClient(self, client: UdsClient):
        if client.ecu_.diag_id not in self.client_list:
            self.client_list[str(client.ecu_.diag_id)] = client
            self.client_list[str(client.ecu_.diag_id)].Start()

    def SendRequest(self, ecu_id, req, callback):
        if ecu_id in self.client_list:
            self.client_list[str(ecu_id)].SendRequest(req,callback)
        
