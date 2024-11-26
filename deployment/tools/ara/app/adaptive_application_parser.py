from __future__ import annotations
from deployment.tools.ara.app.adaptive_application import *
from deployment.tools.ara.app.adaptive_application_db import AdaptiveApplicationDb
class AdaptiveApplicationParser:
    def ParseJson(json_object):
        s_name = list(json_object["adaptive_application"].keys())[0]
        name = ""
        package = json_object["package"]
        if "package" in json_object:
            name = json_object["package"]+"."
        name += s_name
        
        if name in AdaptiveApplicationDb().app_list:
            return
        
        json_object = json_object["adaptive_application"]
        temp_app = AdaptiveApplication(s_name,json_object[s_name]["app"]["bootMode"],AdaptiveApplicationParser.ParseLogger(json_object[s_name]["app"]["logger"]))
        if "parms" in json_object[s_name]["app"]:
            temp_app.parms = json_object[s_name]["app"]["parms"]
        if "provide" in json_object[s_name]:
            for item in json_object[s_name]["provide"]:
               if item["on"] == "ipc" or  item["on"] == "udp":
                    t_name:str = item["name"]
                    orginal_name = t_name
                    if "as" in t_name:
                       temp = t_name.split(" as ")
                       orginal_name = temp[0]
                       t_name = temp[1]
                    print (orginal_name+" -> "+t_name)
                    assert t_name not in temp_app.provide_someip
                    port = ""
                    if "port" in item:
                        port = item["port"]
                    temp_app.provide_someip[t_name] = SomeipItem(item["on"],port,item["instance"],SomeIpDb().FindService(orginal_name,package),"kOut")
               
               elif item["on"] == "diag":
                    t_name:str = item["name"]
                    orginal_name = t_name
                    if "as" in t_name:
                       temp = t_name.split(" as ")
                       orginal_name = temp[0]
                       t_name = temp[1]
                    print (orginal_name+" -> "+t_name)
                    assert t_name not in temp_app.provide_uds
                    temp_app.provide_uds[t_name] = UdsItem(DiagDb().FindJob(orginal_name,package))
                    
               elif item["on"] == "monitor":
                    t_name:str = item["name"]
                    orginal_name = t_name
                    if "as" in t_name:
                       temp = t_name.split(" as ")
                       orginal_name = temp[0]
                       t_name = temp[1]
                    print (orginal_name+" -> "+t_name)
                    assert t_name not in temp_app.provide_dtc
                    temp_app.provide_dtc[t_name] = DtcItem(DiagDb().FindDtc(orginal_name,package))
              
              
        if "require" in json_object[s_name]:
            for item in json_object[s_name]["require"]:
               if item["on"] == "ipc" or  item["on"] == "udp":
                    t_name:str = item["name"]
                    orginal_name = t_name
                    if "as" in t_name:
                       temp = t_name.split(" as ")
                       orginal_name = temp[0]
                       t_name = temp[1]
                    print (orginal_name+" -> "+t_name)
                    assert t_name not in temp_app.provide_someip
                    port = ""
                    if "port" in item:
                        port = item["port"]
                    temp_app.require_someip[t_name] = SomeipItem(item["on"],port,item["instance"],SomeIpDb().FindService(orginal_name,package),"kIn")
               
            #    elif item["on"] == "diag":
            #         t_name:str = item["name"]
            #         orginal_name = t_name
            #         if "as" in t_name:
            #            temp = t_name.split(" as ")
            #            orginal_name = temp[0]
            #            t_name = temp[1]
            #         print (orginal_name+" -> "+t_name)
            #         assert t_name not in temp_app.provide_uds
            #         temp_app.provide_uds[t_name] = UdsItem(DiagDb().FindJob(orginal_name,package))
                    
            #    elif item["on"] == "monitor":
            #         t_name:str = item["name"]
            #         orginal_name = t_name
            #         if "as" in t_name:
            #            temp = t_name.split(" as ")
            #            orginal_name = temp[0]
            #            t_name = temp[1]
            #         print (orginal_name+" -> "+t_name)
            #         assert t_name not in temp_app.provide_dtc
            #         temp_app.provide_dtc[t_name] = DtcItem(DiagDb().FindDtc(orginal_name,package))   
   
        AdaptiveApplicationDb().app_list[name] = temp_app
        
    def ParseLogger(json_object) -> LogerConfig:
        logger = LogerConfig(json_object["app_id"],json_object["app_des"],json_object["log_level"],json_object["log_mode"])
        if "ctx" in json_object:
            for item in json_object["ctx"]:
                des = ""
                if "ctx_des" in item:
                    des = item["ctx_des"]
                logger.ctx_list.append(LoggerCtx(item["ctx_id"],item["log_level"],des))
        return logger
    