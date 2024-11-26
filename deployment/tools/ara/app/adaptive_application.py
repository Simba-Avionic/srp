from __future__ import annotations
from deployment.tools.ara.diag.diag_db import *
from deployment.tools.ara.someip.lib.someip_db import *

class SomeipItem:
    def __init__(self,on:str,port:str,instance:int, item:Service, dir:str) -> None:
        self.on =on
        self.port = port
        self.instance = instance
        self.item = item
        self.dir = dir
        
class DtcItem:
    def __init__(self, item:diag_dtc) -> None:
        self.item = item
        
class UdsItem:
    def __init__(self,item:diag_job):
        self.item = item
class LoggerCtx:
    def __init__(self,ctx_id:str,log_level:str,ctx_des:str) -> None:
        self.ctx_id = ctx_id
        self.log_level = log_level
        self.ctx_des = ctx_des
    def GetCtxId(self) -> str:
        temp_app_id = ""+self.ctx_id
        if len(temp_app_id) < 4:
            for i in range(0,4-len(temp_app_id)):
                temp_app_id+="-"
        return temp_app_id
    def GetLogLevel(self) -> str:
        if "kVerbose" in self.log_level:
            return "log::LogLevel::kVerbose"
        if "kDebug" in self.log_level:
            return "log::LogLevel::kDebug"
        if "kInfo" in self.log_level:
            return "log::LogLevel::kInfo"
        if "kWarn" in self.log_level:
            return "log::LogLevel::kWarn"
        if "kError" in self.log_level:
            return "log::LogLevel::kError"
        
class LogerConfig:
    def __init__(self,app_id:str,app_des:str,log_level:str,log_mode:str) -> None:
        self.app_id = app_id
        self.app_des = app_des
        self.log_level = log_level
        self.log_mode = log_mode
        self.ctx_list: list[LoggerCtx] = []
        
    def GetAppId(self) -> str:
        temp_app_id = ""+self.app_id
        if len(temp_app_id) < 4:
            for i in range(0,4-len(temp_app_id)):
                temp_app_id+="-"
        return temp_app_id
    def GetLogLevel(self) -> str:
        if "kVerbose" in self.log_level:
            return "log::LogLevel::kVerbose"
        if "kDebug" in self.log_level:
            return "log::LogLevel::kDebug"
        if "kInfo" in self.log_level:
            return "log::LogLevel::kInfo"
        if "kWarn" in self.log_level:
            return "log::LogLevel::kWarn"
        if "kError" in self.log_level:
            return "log::LogLevel::kError"
    def GetLogMode(self) -> str:
        res = ""
        if  "kRemote" in self.log_mode:
            if(len(res)==0):
                res =  "log::LogMode::kRemote"
            else:
                res+= " | log::LogMode::kRemote"
        if  "kFile" in self.log_mode:
            if(len(res)==0):
                res =  "log::LogMode::kFile"
            else:
                res+= " | log::LogMode::kFile"
        if  "kConsole" in self.log_mode:
            if(len(res)==0):
                res =  "log::LogMode::kConsole"
            else:
                res+= "|log::LogMode::kConsole"
        if(len(res)==0):
            raise Exception("kRemote , kFile or kConsole not found in log mode")
        return res

class AdaptiveApplication:
    def __init__(self,name:str,bootMode:str,logger:LogerConfig) -> None:
        self.name = name
        self.bootMode = bootMode
        self.logger = logger
        self.parms = ""
        self.startup_delay = 0
        self.provide_someip: dict[str,SomeipItem] = {}
        self.provide_uds: dict[str, UdsItem] = {}
        self.provide_dtc: dict[str, DtcItem] = {}
        self.require_someip: dict[str,SomeipItem] = {}
    def GetDtcList(self) -> dict[str,diag_dtc]:
        res = {}
        for key,val in self.provide_dtc.items():
            res[key] = val.item
        
        return res
    def GetJobList(self) ->dict[str,diag_job]:
        res = {}
        for key,val in self.provide_uds.items():
            res[key] = val.item
        
        return res
    def GetServiceList(self) -> dict[str,SomeipItem]:
        return {**self.provide_someip ,**self.require_someip}