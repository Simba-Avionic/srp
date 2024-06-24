from tools.diag_app.src.lib.db import DB

class QueryMenager:
    def GetEcuList(db: DB):
        res = []
        for ecu in db.ecu_list:
            temp = {}
            temp["name"] = ecu.name 
            temp["ecu_status"] = ecu.ecu_status.value[0]
            temp["diag_id"] = ecu.diag_id 
            temp["ip"] = ecu.ip
            temp["uptime"] = ecu.uptime
            res.append(temp)
        return res