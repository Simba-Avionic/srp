/**
 * @file dtc_database.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-12-23
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef MW_DIAG_DTC_DATABASE_DTC_DATABASE_HPP_
#define MW_DIAG_DTC_DATABASE_DTC_DATABASE_HPP_

#include <chrono>  // NOLINT
#include <string>
#include <unordered_map>

#include "core/common/error_code.h"
#include "core/logger/Logger.h"
#include "mw/diag/dtc/database/dtc_database_element.hpp"
namespace simba {
namespace mw {
namespace dtc {

class DtcDatabase {
 private:
  std::unordered_map<uint16_t, DtcDatabaseElement> errors_{};
  core::logger::Logger logger_;
  uint16_t active_errors;

 public:
  DtcDatabase();
  core::ErrorCode AddError(
      uint16_t service_id, uint16_t error_id, std::string details,
      DtcErrorStatus_t status = DtcErrorStatus_t::kCreated);
  core::ErrorCode ManResetError(uint16_t error_id);
  core::ErrorCode AutoResetError(uint16_t error_id);
  uint16_t ErrorNum();
  std::string GetErrorDetails(uint16_t error_id);
};

}  // namespace dtc
}  // namespace mw
}  // namespace simba
#endif  // MW_DIAG_DTC_DATABASE_DTC_DATABASE_HPP_
