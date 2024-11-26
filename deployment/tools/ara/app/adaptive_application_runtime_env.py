from __future__ import annotations
from deployment.tools.ara.app.adaptive_application import *
class AdaptiveApplicationRuntimeEnv:
    def CreateLogerInit(app:AdaptiveApplication)->str:
        res = """
Result<void> InitializeLogger() noexcept {
    log::LoggingMenager::Create(\""""+app.logger.GetAppId()+"""\","""+app.logger.GetLogMode()+""","""+app.logger.GetLogLevel()+""");

"""
        have_ara = False
        for ctx in app.logger.ctx_list:
            if ctx.GetCtxId() == "ara-":
                have_ara = True
            res+="""    std::ignore = log::LoggingMenager::GetInstance()->CreateLogger(\""""+ctx.GetCtxId()+"""\", \""""+ctx.ctx_des+"""\", """+ctx.GetLogLevel()+""");
"""
        if not have_ara:
            res +="""
    std::ignore = log::LoggingMenager::GetInstance()->CreateLogger("ara-", "Default ctx for ara", log::LogLevel::kInfo);
"""          
        
        res +="""
    return {};
    }
"""
        
        return res