from __future__ import annotations

from deployment.tools.ara.common.data_structure import *
from deployment.tools.ara.common.data_structure_db import DataStructureDB
class DataStructureParser:
    
    def LoadDataStructure(json_obj,db_ = DataStructureDB()):
        name = ""
        endian = "little"
        if "package" in json_obj:
            name = json_obj["package"]
        if "endian" in json_obj:
            endian = json_obj["endian"]
        assert "data_structure" in json_obj
        temp_list = list(json_obj["data_structure"].keys())
        if len(name) > 0:
            name += ("."+temp_list[0])
        else:
            name = temp_list[0]
        temp_struct: Structure = Structure(name)
        temp_struct.endian = endian
        for v_name,type in json_obj["data_structure"][temp_list[0]].items():
            temp_struct.AddVariable(v_name,db_.CreateVariable(v_name,json_obj["package"],type))
        if name not in db_.data_structure:
            db_.data_structure[name] = temp_struct
        else:
            raise Exception(f"Data Structure ({name}) already exist")
        
