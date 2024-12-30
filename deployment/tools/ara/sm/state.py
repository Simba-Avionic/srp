from __future__ import annotations
import hashlib

class Transition:
    def __init__(self,desc):
        self.desc = desc
    def GetCode(self,db):
        return ""

class EmptyTransition(Transition):
    def __init__(self):
        super().__init__("")
    def GetCode(self,db):
        return """// Empty"""

class SwitchTransition(Transition):
    def __init__(self,desc):
        super().__init__(desc)
    def GetCode(self,db):
        item = db.states[self.desc]
        return f"""this->SwitchState({item.id}U);"""
class DtcTransition(Transition):
    def __init__(self, mode:bool):
        super().__init__("")
        self.mode = mode
    def GetCode(self,db):
        if self.mode:
            return f"""this->DtcSetStatus(true);"""
        else:
            return f"""this->DtcSetStatus(false);"""
class MultipleTransition(Transition):
    def __init__(self,t_list:list[Transition]):
        super().__init__("")
        self.t_list = t_list
    def GetCode(self,db):
        out = "";
        for k in self.t_list:
            out+=k.GetCode(db)+"\n"
        return out;
class Mode:
    def __init__(self,restart:bool, shutdown: bool):
        self.restart = restart
        self.shutdown = shutdown

class State:
    def __init__(self, name:str, mode:Mode ,FinishDo:Transition, ErrorDo:Transition,DiDDo:dict[int,Transition]):
        self.name = name
        self.mode = mode
        self.FinishDo = FinishDo
        self.ErrorDo = ErrorDo
        self.DiDDo = DiDDo
        self.id = int(hashlib.shake_128(self.name.encode('utf-8')).hexdigest(2), 16)