from __future__ import annotations
from deployment.tools.ara.common.data_structure import data_type
class Endpoint:
    def __init__(self,name:str,id:int,in_parm:data_type, out_parm:data_type) -> None:
        self.name:str = name
        self.id:int = id
        self.in_parm:data_type = in_parm
        self.out_parm:data_type = out_parm
        
class Method(Endpoint):
    def __init__(self, name: str, id: int, in_parm: data_type, out_parm: data_type) -> None:
        assert id < 0x8000
        super().__init__(name, id, in_parm, out_parm)
    
class Event(Endpoint):
    def __init__(self, name: str, id: int, out_parm: data_type) -> None:
        assert id >= 0x8000
        super().__init__(name, id, None, out_parm)
    
class Service:
    def __init__(self):
        self.name = ""
        self.id:int = 0
        self.major:int = 0
        self.minor:int = 0
        self.package:str = ""
        self.methods:list[Method] = []
        self.events:list[Event] = []
        self.dir = "None"