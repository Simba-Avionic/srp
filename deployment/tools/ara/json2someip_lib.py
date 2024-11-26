from __future__ import annotations
import sys
import os
import copy
import json
from deployment.tools.ara.common.common_parser import CommonParser
from deployment.tools.ara.app.adaptive_application_db import AdaptiveApplicationDb
from deployment.tools.ara.app.adaptive_application_extractor import AdaptiveApplicationExtractor
from deployment.tools.ara.common.data_structure_extractor import *
from deployment.tools.ara.someip.lib.someip_extractor import SomeipExtractor
from deployment.tools.ara.common.data_structure import data_type
from deployment.tools.ara.someip.lib.someip_db import *
def LoadJson(path:str):
    CommonParser.LoadJson(path)
def CreateDir(start:str,finish:str):
    for p in finish.split("/"):
            start+="/"+p
            try:
                os.makedirs(start)
            except:
                pass
def ExtractStructList(struct_list: dict[str,data_type],s:data_type):
    struct_list[s.name] = s
    for key,d in s.variable_list.items():
        if d.typ_str == "struct":
            ExtractStructList(struct_list,d)
if __name__ == "__main__":
    out_path = sys.argv[1]
    src_path = ""
    for val in sys.argv:
        if ".json" in val:
            if os.path.isfile(val):
                with open(val,"r") as file:
                    json_obj = json.load(file)
                    if "adaptive_application" in json_obj:
                        if len(src_path) == 0:
                            src_path = val
                        else:
                            raise Exception("Multiple Adaptive App model found!")
    if len(src_path) != 0:                
        LoadJson(src_path)
        app_name = list(AdaptiveApplicationDb().app_list.keys())[0]
        app = AdaptiveApplicationDb().app_list[list(AdaptiveApplicationDb().app_list.keys())[0]]
        # for p in app_name.split("."):
        #     out_path+="/"+p
        #     os.makedirs(out_path)
        service_list: dict[str,Service] = {}
        struct_list: dict[str,data_type] = {}
        for k,s in app.provide_someip.items():
            if s.item.name+"kOut" not in service_list:
                CreateDir(copy.copy(out_path),s.item.name.replace(".","/"))
                service_list[s.item.name+"kOut"] = copy.copy(s.item)
                service_list[s.item.name+"kOut"].dir = "kOut"
                CreateDir(copy.copy(out_path),s.item.name.replace(".","/").replace("/"+s.item.name.split(".")[-1],""))
                for m in s.item.methods:
                    if m.in_parm.typ_str == "struct":
                        if m.in_parm.name not in struct_list:
                            ExtractStructList(struct_list,m.in_parm)
                            # struct_list[m.in_parm.name] = m.in_parm
                            CreateDir(copy.copy(out_path),m.in_parm.name.replace(".","/").replace("/"+m.in_parm.name.split(".")[-1],""))
                    if m.out_parm.typ_str == "struct":
                        if m.out_parm.name not in struct_list:
                            ExtractStructList(struct_list,m.out_parm)
                            CreateDir(copy.copy(out_path),m.out_parm.name.replace(".","/").replace("/"+m.out_parm.name.split(".")[-1],""))
                for m in s.item.events:
                    if m.out_parm.typ_str == "struct":
                        if m.out_parm.name not in struct_list:
                            ExtractStructList(struct_list,m.out_parm)
                            CreateDir(copy.copy(out_path),m.out_parm.name.replace(".","/").replace("/"+m.out_parm.name.split(".")[-1],""))
        for k,s in app.require_someip.items():
            if s.item.name+"kIn" not in service_list:
                CreateDir(copy.copy(out_path),s.item.name.replace(".","/"))
                service_list[s.item.name+"kIn"] = copy.copy(s.item)
                service_list[s.item.name+"kIn"].dir = "kIn"
                CreateDir(copy.copy(out_path),s.item.name.replace(".","/").replace("/"+s.item.name.split(".")[-1],""))
                for m in s.item.methods:
                    if m.in_parm.typ_str == "struct":
                        if m.in_parm.name not in struct_list:
                            ExtractStructList(struct_list,m.in_parm)
                            # struct_list[m.in_parm.name] = m.in_parm
                            CreateDir(copy.copy(out_path),m.in_parm.name.replace(".","/").replace("/"+m.in_parm.name.split(".")[-1],""))
                    if m.out_parm.typ_str == "struct":
                        if m.out_parm.name not in struct_list:
                            ExtractStructList(struct_list,m.out_parm)
                            CreateDir(copy.copy(out_path),m.out_parm.name.replace(".","/").replace("/"+m.out_parm.name.split(".")[-1],""))
                for m in s.item.events:
                    if m.out_parm.typ_str == "struct":
                        if m.out_parm.name not in struct_list:
                            ExtractStructList(struct_list,m.out_parm)
                            CreateDir(copy.copy(out_path),m.out_parm.name.replace(".","/").replace("/"+m.out_parm.name.split(".")[-1],""))
        # for k,v in service_list.items():
        #     print(k)
        for k,v in struct_list.items():
            DataStructureExtractor.ExtractStructure(out_path,v)
        for k,v in service_list.items():
            print(k)
            print(v.dir)
            SomeipExtractor.ExtractService(out_path,v)
    else:
        raise Exception("Adaptive app def not found!")