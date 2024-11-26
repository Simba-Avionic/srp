from __future__ import annotations
import sys
import os
import json
from deployment.tools.ara.common.common_parser import CommonParser
from deployment.tools.ara.app.adaptive_application_db import AdaptiveApplicationDb
from deployment.tools.ara.app.adaptive_application_extractor import AdaptiveApplicationExtractor

def LoadJson(path:str):
    CommonParser.LoadJson(path)

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
        with open(out_path+"/logger.json","w") as out_file:
            json.dump(AdaptiveApplicationExtractor.CreateLoggerJson(AdaptiveApplicationDb().app_list[list(AdaptiveApplicationDb().app_list.keys())[0]]),out_file,indent=2)
        with open(out_path+"/srp_app.json","w") as out_file:
            json.dump(AdaptiveApplicationExtractor.CreateSrpAppJson(AdaptiveApplicationDb().app_list[list(AdaptiveApplicationDb().app_list.keys())[0]]),out_file,indent=2)
            
    else:
        raise Exception("Adaptive app def not found!")