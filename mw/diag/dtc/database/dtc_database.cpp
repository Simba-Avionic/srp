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

DtcDatabase::DtcDatabase():error_id{0},active_errors{0}{
    //TODO init logger
}
core::ErrorCode DtcDatabase::AddError(uint16_t dtc_error_code_,std::string details_,
                            uint8_t status_){
    this->errors_.insert({error_id,DtcDatabaseElement(dtc_error_code_,details_,status_)});
    this->error_id+=1;
    this->active_errors+=1;
    return core::ErrorCode::kOk;
}
core::ErrorCode DtcDatabase::ManResetError(uint16_t error_id){
    auto error=this->errors_.find(error_id);
    if (error==this->errors_.end()){
        return core::ErrorCode::kNotDefine;
    }
    error->second.SetStatus(0x2);
    this->active_errors-=1;
    return core::ErrorCode::kOk;
}
core::ErrorCode DtcDatabase::AutoResetError(uint16_t error_id){
    auto error=this->errors_.find(error_id);
    if (error==this->errors_.end()){
        return core::ErrorCode::kNotDefine;
    }
    error->second.SetStatus(0x1);
    this->active_errors-=1;
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
