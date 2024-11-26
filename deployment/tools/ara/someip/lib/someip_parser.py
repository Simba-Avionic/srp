from __future__ import annotations
from deployment.tools.ara.someip.lib.someip_db import SomeIpDb
from deployment.tools.ara.someip.lib.service import *
from deployment.tools.ara.common.data_structure_db import DataStructureDB

import json

class SomeIpParser:
    def LoadJson(path:str):
        with open(path,"r") as json_file:
            json_obj = json.load(json_file)
            SomeIpParser.ParseJson(json_obj)
            
    def ParseJson(json_obj, db_ = SomeIpDb()):
            if "someip" in json_obj:
                SomeIpParser.LoadSomeipService(json_obj,db_)
                

    def LoadSomeipService(json_obj,db_:SomeIpDb):
        name = ""
        if "package" in json_obj:
            name = json_obj["package"]
        assert "someip" in json_obj
        temp_list = list(json_obj["someip"].keys())
        if len(name) > 0:
            name += ("."+temp_list[0])
        else:
            name = temp_list[0]
        temp_service = Service()
        temp_service.name = name
        temp_service.id = json_obj["someip"][temp_list[0]]["service_id"]
        temp_service.major = json_obj["someip"][temp_list[0]]["major_version"]
        temp_service.minor = json_obj["someip"][temp_list[0]]["minor_version"]
        if "methods" in json_obj["someip"][temp_list[0]]:
            temp_service.methods.extend(SomeIpParser.ExportMethodsList(json_obj["package"],json_obj["someip"][temp_list[0]]["methods"],db_))
        if "events" in json_obj["someip"][temp_list[0]]:
            temp_service.events.extend(SomeIpParser.ExportEventsList(json_obj["package"],json_obj["someip"][temp_list[0]]["events"],db_))
        SomeIpDb().service_list[name] = temp_service
            
        
    def ExportMethodsList(package:str,method_json_list,db_:SomeIpDb) -> list[Method]:
        res: list[Method] = []
        for key, json_object in method_json_list.items():
            res.append(SomeIpParser.ExportMethod(key,package,json_object,db_))
        return res

    def ExportEventsList(package:str,method_json_list,db_:SomeIpDb) -> list[Event]:
        res: list[Event] = []
        for key, json_object in method_json_list.items():
            res.append(SomeIpParser.ExportEvent(key,package,json_object,db_))
        return res

    def ExportMethod(name:str,package:str,json_object,db_:SomeIpDb) -> Method:
        input_parm = DataStructureDB().CreateVariable("in_parm",package,json_object["data_structure"]["in"]["type"])
        output_parm = DataStructureDB().CreateVariable("out",package,json_object["data_structure"]["out"]["type"])
        return Method(name,json_object["id"],input_parm,output_parm)
    
    def ExportEvent(name:str,package:str,json_object,db_:SomeIpDb) -> Event:
        output_parm = DataStructureDB().CreateVariable("out",package,json_object["data_structure"]["out"]["type"])
        return Event(name,json_object["id"],output_parm)