from __future__ import annotations
from deployment.tools.ara.diag.diag_db import DiagDb
from deployment.tools.ara.diag.diag_jobs import *
from deployment.tools.ara.diag.diag_dtc import diag_dtc
class DiagParser:
    def ParseJson(json_object):
        package = ""
        if "package" in json_object:
            package = json_object["package"]
        if "global" in json_object["diag"]:
            DiagParser.ParseGlobalJob(json_object["diag"]["global"],package)
        if "job" in json_object["diag"]:
            DiagParser.ParseJob(json_object["diag"]["job"],package)
        if "dtc" in json_object["diag"]:
            DiagParser.ParseDtc(json_object["diag"]["dtc"],package)
    def ParseGlobalJob(json_object, package:str):
        s_name = list(json_object.keys())[0]
        key = package+s_name
        # assert key not in DiagDb().global_jobs, f"Global Job exist in DB ({key})"
        if key in DiagDb().global_jobs:
            return
        DiagDb().global_jobs[key] = diag_job_global(json_object[s_name]["service_id"],json_object[s_name]["sub_service_max_size"],json_object[s_name]["sub_service_min_size"])
    
    def ParseJob(json_object, package:str):
        s_name = list(json_object.keys())[0]
        key = package+s_name
        if key in DiagDb().jobs:
            return
        # assert key not in DiagDb().jobs, f"Job exist in DB ({key})"
        
        desc = ""
        if "desc" in json_object[s_name]:
            desc = json_object[s_name]["desc"]
            
        DiagDb().jobs[key]=diag_job(DiagDb().global_jobs[json_object[s_name]["type"]].service_id,json_object[s_name]["sub_service_id"],desc)
    
    def ParseDtc(json_object, package:str):
        s_name = list(json_object.keys())[0]
        key = package+s_name
        # assert key not in DiagDb().dtcs, f"DTC exist in DB ({key})"
        id = 0
        desc = ""
        if isinstance(json_object[s_name]["id"],str):
            id = int(json_object[s_name]["id"],16)
        else:
            id = json_object[s_name]["id"]
        DiagDb().dtcs[key] = diag_dtc(id,desc)