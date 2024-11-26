from __future__ import annotations
from deployment.tools.ara.someip.lib.service import Event, Method, Service
from deployment.tools.ara.common.singleton_lib import SingletonMeta
class SomeIpDb(metaclass=SingletonMeta):
    def __init__(self) -> None:
        self.service_list: dict[str,Service] = {}
    def FindService(self,key,package) -> Service:
        if key in self.service_list:
            return self.service_list[key]
        if (package+"."+key) in self.service_list:
            return self.service_list[(package+"."+key)]
        raise Exception("Service: "+key+" or "+(package+"."+key)+" not found")