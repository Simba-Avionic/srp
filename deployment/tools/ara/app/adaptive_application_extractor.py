from __future__ import annotations
from deployment.tools.ara.app.adaptive_application import AdaptiveApplication
class AdaptiveApplicationExtractor:
    def CreateLoggerJson(app: AdaptiveApplication):
        out_json = {}
        
        out_json["app_id"] = app.logger.app_id
        out_json["app_des"] = app.logger.app_des
        out_json["log_level"] = app.logger.log_level
        out_json["log_mode"] = app.logger.log_mode
        
        return out_json
    def CreateSrpAppJson(app: AdaptiveApplication):
        out_json = {}
        out_json["app_name"] = app.name
        out_json["app_id"] = app.id
        out_json["bin_path"] = f"/srp/opt/{app.name.split(".")[-1]}/bin/{app.name.split(".")[-1]}"
        out_json["parms"] = app.parms
        out_json["fg_list"] = app.functional_groups
        return out_json