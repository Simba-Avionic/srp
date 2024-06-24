import json
from enum import Enum
from threading import Lock

class SingletonMeta(type):
    """
    This is a thread-safe implementation of Singleton.
    """

    _instances = {}

    _lock: Lock = Lock()
    """
    We now have a lock object that will be used to synchronize threads during
    first access to the Singleton.
    """

    def __call__(cls, *args, **kwargs):
        """
        Possible changes to the value of the `__init__` argument do not affect
        the returned instance.
        """
        # Now, imagine that the program has just been launched. Since there's no
        # Singleton instance yet, multiple threads can simultaneously pass the
        # previous conditional and reach this point almost at the same time. The
        # first of them will acquire lock and will proceed further, while the
        # rest will wait here.
        with cls._lock:
            # The first thread to acquire the lock, reaches this conditional,
            # goes inside and creates the Singleton instance. Once it leaves the
            # lock block, a thread that might have been waiting for the lock
            # release may then enter this section. But since the Singleton field
            # is already initialized, the thread won't create a new object.
            if cls not in cls._instances:
                instance = super().__call__(*args, **kwargs)
                cls._instances[cls] = instance
        return cls._instances[cls]

class ECUStatus(Enum):
    OFFLINE = 1,
    ONLINE = 2,
    ERROR = 3

class DiagJob:
    def __init__(self,name,s_id,sub_id,desc) -> None:
       self.name = name
       self.s_id = s_id
       self.sub_id = sub_id
       self.desc = desc
class Dtc:
    def __init__(self,name,id,desc,snapshot = []) -> None:
        self.name = name
        self.id = id
        self.desc = desc
        self.snapshot = snapshot
       
class ECU:
 def __init__(self,ip,name,diag_id) -> None:
    self.ip = ip
    self.name = name
    self.diag_id = diag_id
    self.ecu_status = ECUStatus.OFFLINE
    self.uptime = 0
    self.diag_jobs = []
    self.dtc = []
class DB(metaclass=SingletonMeta):
    def __init__(self) -> None:
        self.ecu_list = []
    def ExtractEcu(json_obj) -> ECU:
        return ECU(json_obj["ip"],json_obj["sname"],int(json_obj["logic_address"]))
    def ExtractJob(json_obj) -> DiagJob:
        return DiagJob(json_obj["name"],json_obj["s_id"],json_obj["sub_id"],json_obj["desc"])
    def ExtractJobList(json_obj, ecu):
        for job in json_obj:
            item = DB.ExtractJob(job)
            ecu.diag_jobs.append(item)
    def ExtractDtc(json_obj)->Dtc:
        return Dtc(json_obj["name"],json_obj["id"],json_obj["desc"],json_obj["snapshot"])
    def ExtractDtcList(json_obj,ecu):
        for dtc in json_obj:
            item = DB.ExtractDtc(dtc)
            ecu.dtc.append(item)
    def load(self,path:str):
        with open(path,"r") as input_file:
            obj_list = json.loads(input_file.read())
            for obj in obj_list:
                ecu = DB.ExtractEcu(obj["platform"])
                DB.ExtractJobList(obj["jobs"],ecu)
                self.ecu_list.append(ecu)