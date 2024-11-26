from __future__ import annotations


class data_type:
    def __init__(self,name:str,type_str:str,size:int = 0,extra_cpp_lib:list[str] = [],is_struc = False) -> None:
        self.name:str = name
        self.typ_str:str = type_str
        self.extra_cpp_lib:list[str] = extra_cpp_lib
        self.variable_list: dict[str,data_type] = {}
        self.size = size
        self.endian = "big"
    def DefineVariable(self) -> str:
        return self.typ_str+" "+self.name+";"
    def DeclareVariable(self) -> str:
        return ""
    def GetAsParm(self) -> str:
        return self.typ_str+" "+self.name
    def GetInclude(self) -> list[str]:
        res:list[str] = []
        for include in self.extra_cpp_lib:
            res.append("#include " + include)
        return res
    def GetTypeOnly(self) -> str:
        return self.typ_str
    def GetSize(self) -> int:
        return self.size
class Int8(data_type):
    def __init__(self,name:str) -> None:
        super().__init__(name,"std::int8_t",1,["<cstdint>"])
        
class Int16(data_type):
    def __init__(self,name:str) -> None:
        super().__init__(name,"std::int16_t",2,["<cstdint>"])
        
class Int32(data_type):
    def __init__(self,name:str) -> None:
        super().__init__(name,"std::int32_t",4,["<cstdint>"])
        
class Int64(data_type):
    def __init__(self,name:str) -> None:
        super().__init__(name,"std::int64_t",8,["<cstdint>"])
        
class Uint8(data_type):
    def __init__(self,name:str) -> None:
        super().__init__(name,"std::uint8_t",1,["<cstdint>"])
        
class Uint16(data_type):
    def __init__(self,name:str) -> None:
        super().__init__(name,"std::uint16_t",2,["<cstdint>"])
        
class Uint32(data_type):
    def __init__(self,name:str) -> None:
        super().__init__(name,"std::uint32_t",4,["<cstdint>"])
        
class Uint64(data_type):
    def __init__(self,name:str) -> None:
        super().__init__(name,"std::uint64_t",8,["<cstdint>"])
        
class Float32(data_type):
    def __init__(self,name:str) -> None:
        super().__init__(name,"float",4)      
 
class Float64(data_type):
    def __init__(self,name:str) -> None:
        super().__init__(name,"double",8)

class Bool(data_type):
    def __init__(self,name:str) -> None:
        super().__init__(name,"bool",1)       
class Void(data_type):
    def __init__(self,name:str) -> None:
        super().__init__(name,"void",)     
class String(data_type):
    def __init__(self,name:str) -> None:
        super().__init__(name,"std::string",0, ["<string>"])           
class Structure(data_type):
    def __init__(self, name: str) -> None:
        path = name.replace(".","/")
        super().__init__(name, "struct",0,[])
        
    def AddVariable(self, name:str,variable:data_type):
        if name not in self.variable_list:
            self.variable_list[name] = variable
        else:
            assert False
    def GetSize(self) -> int:
        res = 0
        for k,v in self.variable_list.items():
            res+=v.GetSize()
        return res