/**
 * @file dtcService.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-12-23
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "mw/diag/dtc/service/dtcService.h"

#include <unordered_map>
#include <vector>

#include "core/json/json_parser.h"
#include "core/logger/Logger.h"

namespace simba {
namespace mw {
namespace dtc {

DtcService::DtcService() {}
core::ErrorCode DtcService::Initialize(
    const std::unordered_map<std::string, std::string>& parms) {
  db_ = std::make_shared<DataBase>();
  dtc_list_ = std::make_unique<DtcListJob>("DtcService/DtcList", db_);
  dtc_by_mask_job_ =
      std::make_unique<DtcByMaskJob>("DtcService/DtcByMask", db_);
  dtc_snapshot_job_ =
      std::make_unique<DtcSnapshotJob>("DtcService/dtc_snapshot");
  clear_dtc_job_ =
      std::make_unique<ClearDtcJob>("DtcService/clear_memory", db_);
  this->LoadConfig("/opt/" + parms.at("app_name") +
                   "/etc/dtc_support_list.json");
  return core::ErrorCode::kOk;
}

core::ErrorCode DtcService::Run(std::stop_token token) {
  dtc_list_->StartService();
  dtc_by_mask_job_->StartService();
  dtc_snapshot_job_->StartService();
  clear_dtc_job_->StartService();
  AppLogger::Info("App started");
  this->SleepMainThread();
  return core::ErrorCode::kOk;
}

void DtcService::LoadConfig(const std::string& path) {
  const auto json_obj_t = core::json::JsonParser::Parser(path);
  if (!json_obj_t.has_value()) {
    AppLogger::Error("No config file!");
    this->db_->InsertElement(DataBaseElement{0xFFAAAA, "0xFFAAAA", {}});
    return;
  }
  const auto db_json_t = (json_obj_t.value()).GetArray("db");
  if (!db_json_t.has_value()) {
    AppLogger::Error("Incorrect JSON structure!");
    this->db_->InsertElement(DataBaseElement{0xFFAAAA, "0xFFAAAA", {}});
    return;
  }
  const auto db_hash_t = (json_obj_t.value()).GetString("hash");
  for (const auto& dtc : db_json_t.value()) {
    const auto name_t = dtc.GetString("name");
    const auto id_t = dtc.GetNumber<uint32_t>("id");
    const auto snapshot_t = dtc.GetArray("snapshot");
    if (!name_t.has_value()) {
      AppLogger::Error("DTC don't have name!");
      break;
    }
    if (!id_t.has_value()) {
      AppLogger::Error("DTC don't have id!");
      break;
    }
    if (!snapshot_t.has_value()) {
      AppLogger::Error("DTC don't have snapshot!");
      break;
    }
    {
      const auto name = name_t.value();
      const auto id = id_t.value();
      std::vector<SnapshotService> services{};
      for (const auto& snap_service : snapshot_t.value()) {
        const auto s_address_t = snap_service.GetString("adress");
        const auto s_name_t = snap_service.GetNumber<uint16_t>("name");
        if (!s_address_t.has_value()) {
          AppLogger::Error("Snapshot don't have address!");
          break;
        }
        if (!s_name_t.has_value()) {
          AppLogger::Error("Snapshot don't have name!");
          break;
        }
        services.push_back(
            SnapshotService{s_address_t.value(), s_name_t.value()});
      }
      db_->InsertElement(DataBaseElement{id, name, std::move(services)});
    }
  }

  if (db_->GetDataBase().size() == 0) {
    AppLogger::Error("Empty DB !");
    this->db_->InsertElement(DataBaseElement{0xFFAAAA, "0xFFAAAA", {}});
  } else {
    if (db_hash_t.has_value()) {
      db_->SetHash(db_hash_t.value());
      AppLogger::Info("DB hash: " + db_hash_t.value());
    } else {
      AppLogger::Error("DB haven't hash");
    }
  }
}
}  // namespace dtc
}  // namespace mw
}  // namespace simba
