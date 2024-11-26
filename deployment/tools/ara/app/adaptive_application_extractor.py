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
        out_json["startup_prio"] = app.bootMode
        out_json["parms"] = app.parms
        out_json["startup_after_delay"] = app.startup_delay
        out_json["bin_path"] = f"/opt/{app.name}/bin/{app.name}"
        return out_json