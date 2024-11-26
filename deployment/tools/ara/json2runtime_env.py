from __future__ import annotations
import sys
import os
import json
from datetime import date
from deployment.tools.ara.common.common_parser import CommonParser
from deployment.tools.ara.app.adaptive_application_db import AdaptiveApplicationDb
from deployment.tools.ara.app.adaptive_application_extractor import AdaptiveApplicationExtractor
from deployment.tools.ara.app.adaptive_application_runtime_env import AdaptiveApplicationRuntimeEnv
from deployment.tools.ara.diag.diag_runtime_env import DiagRuntimeEnv
from deployment.tools.ara.someip.lib.someip_runtime_env import SomeipRuntimeEnv

file_content_core = """/**
 * @file initialization.cc
 * ARA Env Generator by Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date """+str(date.today())+""""
 * 
 * @copyright Copyright (c) 2024
 * 
 * !! Automatically generated file please do not change anything !!
 *
 */

#include "ara/core/initialization.h"
#include "ara/com/initialization.h"
#include <stop_token>

#include "ara/core/model/com_model_item.h"
#include "ara/core/model/diag_model_item.h"
#include "ara/core/model_db.h"
#include "ara/log/logging_menager.h"

namespace ara {
namespace core {
namespace {
    std::stop_source ara_stop_source_{};
}
Result<void> InitializeDiagDb(ModelDataBase& db_) noexcept;
Result<void> InitializeComDb(ModelDataBase& db_) noexcept;
Result<void> InitializeLogger() noexcept;

Result<void> Initialize() noexcept {
  std::ignore = InitializeLogger();
  const auto ara_logger_ = log::LoggingMenager::GetInstance()->CreateLogger("ara-");
  ara_logger_.LogInfo() << "Logger initialized";
  {
    auto& db_ = ModelDataBase::GetInstance();
    ara_logger_.LogInfo() << "Instances of the model database have been created";
    std::ignore = InitializeDiagDb(db_);
    ara_logger_.LogInfo() << "Added models associated with ara:diag";
    std::ignore = InitializeComDb(db_);
    ara_logger_.LogInfo() << "Added models associated with ara:com";
  }
  
  ara_logger_.LogInfo() << "ARA environment has been initialized";
  ara::com::Initialize();
  ara_logger_.LogInfo() << "ARA::COM environment has been initialized";
  return {};
}

Result<void> Deinitialize() noexcept {
  ara_stop_source_.request_stop();
  const auto ara_logger_ = log::LoggingMenager::GetInstance()->CreateLogger("ara-");
  ara_logger_.LogInfo() << "ARA environment has been deinitialized";
  
  return {}; 
}

"""

file_content_core_end = """}  // namespace core
}  // namespace ara
"""

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
        temp_s = ""
        temp_s = DiagRuntimeEnv.CreateInitDB(list(AdaptiveApplicationDb().app_list.keys())[0].replace(".","/"),
                                             AdaptiveApplicationDb().app_list[list(AdaptiveApplicationDb().app_list.keys())[0]].GetDtcList(),
                                             AdaptiveApplicationDb().app_list[list(AdaptiveApplicationDb().app_list.keys())[0]].GetJobList())
        temp_s+="\n"
        temp_s+= SomeipRuntimeEnv.CreateInitDB(list(AdaptiveApplicationDb().app_list.keys())[0].replace(".","/"),
                                               AdaptiveApplicationDb().app_list[list(AdaptiveApplicationDb().app_list.keys())[0]].GetServiceList())
        temp_s+="\n"
        temp_s+=AdaptiveApplicationRuntimeEnv.CreateLogerInit(AdaptiveApplicationDb().app_list[list(AdaptiveApplicationDb().app_list.keys())[0]])
        with open(out_path+"/initialization.cc","w") as out_file:
            out_file.write(file_content_core+temp_s+file_content_core_end)
    else:
        raise Exception("Adaptive app def not found!")