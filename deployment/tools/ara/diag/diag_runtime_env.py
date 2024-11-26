from __future__ import annotations
from deployment.tools.ara.diag.diag_dtc import diag_dtc
from deployment.tools.ara.diag.diag_jobs import diag_job

class DiagRuntimeEnv:
  def CreateInitDB(app_name:str,dtc_list:dict[str,diag_dtc], job_list:dict[str,diag_job]) -> str:
    res:str = """
Result<void> InitializeDiagDb(ModelDataBase& db_) noexcept {
"""
    for key,dtc in dtc_list.items():
          hex_id = hex(dtc.id)
          hex_id = hex_id.upper()
          hex_id = hex_id.replace("X","x")
          res+= """
  {
    const auto res = ara::core::model::ModelDtc::CreatItem(\""""+hex_id+"""\","""+str(dtc.id)+""");
    std::ignore = db_.AddNewItem(\"/"""+app_name+"/"+key+"""\",{res.Value()});
  }
          """
    for key, job in job_list.items():
      res +="""
  {
    const auto res = ara::core::model::ModelUds::CreatItem("""+str(job.s_id)+"""U, """+str(job.sub_id)+"""U, \""""+str(int(job.s_id<<16)+job.sub_id)+"""\",ara::core::model::ModelUds::Direction::kOut);
    std::ignore = db_.AddNewItem(\"/"""+app_name+"/"+key+"""\",{res.Value()});
  }
      """
    res += """
  return {};
}
"""
    return res