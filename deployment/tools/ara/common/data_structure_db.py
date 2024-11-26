from __future__ import annotations

from deployment.tools.ara.common.data_structure import *
from deployment.tools.ara.common.singleton_lib import SingletonMeta

class DataStructureDB(metaclass=SingletonMeta):
    def __init__(self) -> None:
        self.data_structure: dict[str,data_type] = {}
        
    def CreateVariable(self,name:str,package:str,v_type:str) -> data_type:
        if v_type == "uint8":
            return Uint8(name)
        elif v_type == "uint16":
            return Uint16(name)
        elif v_type == "uint32":
            return Uint32(name)
        elif v_type == "uint64":
            return Uint64(name)
        elif v_type == "int8":
            return Int8(name)
        elif v_type == "int16":
            return Int16(name)
        elif v_type == "int32":
            return Int32(name)
        elif v_type == "int64":
            return Int64(name)
        elif v_type == "float32":
            return Float32(name)
        elif v_type == "float64":
            return Float64(name)
        elif v_type == "bool":
            return Bool(name)
        elif v_type == "string":
            return String(name)
        elif v_type == "void":
            return Void(name)
        else:
            if package+"."+v_type in self.data_structure:
                return self.data_structure[package+"."+v_type]
            elif v_type in self.data_structure:
                return self.data_structure[v_type]
            else:
                assert False, package+"."+v_type +" or "+v_type+" Not found"