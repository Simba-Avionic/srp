from __future__ import annotations
from deployment.tools.ara.someip.lib.someip_db import *
from deployment.tools.ara.app.adaptive_application import *
class SomeipRuntimeEnv:
    def CreateInitDB(app_name:str,service_list:dict[str,SomeipItem]) -> str:
        res:str = """
Result<void> InitializeComDb(ModelDataBase& db_) noexcept {
"""
        for key,item in service_list.items():
            res+="""
    {"""
            if item.dir == "kOut":
                res+="""
        const auto res = ara::core::model::ModelSomeIp::CreatSkeletonItem("""
            else:
                res+="""
        const auto res = ara::core::model::ModelSomeIp::CreatProxyItem("""
            if item.on.lower() == "ipc":
                res+="""\"someip_ipc_"""+str(item.item.id)+"""_"""+str(item.instance)+"""\",0,"""+str(item.instance)+"""U,"""+str(item.item.major)+"""U,"""+str(item.item.minor)+"""U);
        std::ignore = db_.AddNewItem(\""""+app_name+"/"+key+"""\", {res.Value()});
    }
            """
            elif item.on.lower() == "udp":
                res+="""\"0.0.0.0\","""+item.port+""","""+str(item.instance)+"""U,"""+str(item.item.major)+"""U,"""+str(item.item.minor)+"""U);
        std::ignore = db_.AddNewItem(\""""+app_name+"/"+key+"""\", {res.Value()});
    }
            """
        res += """
  return {};
}
"""
        return res