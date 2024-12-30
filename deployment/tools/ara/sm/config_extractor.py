from __future__ import annotations
import json
import copy
 
from deployment.tools.ara.sm.db import DB

class ConfigExtractor:
    def ExtractConfigFile(file_path:str, db:DB):
        json_obj = {}
        json_obj["states"] = []
        for name, state in db.states.items():
            temp_obj = {}
            temp_obj["name"] = name
            temp_obj["id"] = state.id
            json_obj["states"].append(copy.copy(temp_obj))
        with open(file_path,"w") as file:
            json.dump(json_obj,file,indent=2)