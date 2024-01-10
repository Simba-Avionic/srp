/**
 * @file dtc_database.cpp
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-01-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "mw/diag/dtc/database/dtc_database.hpp"

#include <memory>
namespace simba {
namespace mw {
namespace dtc {

DtcDatabase::DtcDatabase() {
  this->active_errors = 0;
  // TODO(Mateusz): init logger
}
core::ErrorCode DtcDatabase::AddError(
    uint16_t service_id, uint16_t error_id, std::string details,
    DtcErrorStatus_t status = DtcErrorStatus_t::kCreated) {
  this->errors_.insert(
      {error_id, DtcDatabaseElement(service_id, details, status)});
  this->active_errors += 1;
  return core::ErrorCode::kOk;
}
core::ErrorCode DtcDatabase::ManResetError(uint16_t error_id) {
  auto error = this->errors_.find(error_id);
  if (error == this->errors_.end()) {
    return core::ErrorCode::kNotDefine;
  }
  error->second.SetStatus(DtcErrorStatus_t::kManReset);
  this->active_errors -= 1;
  return core::ErrorCode::kOk;
}
core::ErrorCode DtcDatabase::AutoResetError(uint16_t error_id) {
  auto error = this->errors_.find(error_id);
  if (error == this->errors_.end()) {
    return core::ErrorCode::kNotDefine;
  }
  this->active_errors -= 1;
  error->second.SetStatus(DtcErrorStatus_t::kAutoReset);
  return core::ErrorCode::kOk;
}
uint16_t DtcDatabase::ErrorNum() { return this->active_errors; }
std::string DtcDatabase::GetErrorDetails(uint16_t error_id) {
  auto error = this->errors_.find(error_id);
  if (error != this->errors_.end()) {
    return error->second.GetDetails();
  }
  return "";
}

}  // namespace dtc
}  // namespace mw
}  // namespace simba
