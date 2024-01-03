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

#ifndef _MW_DTC_DATABASE_HPP_
#define _MW_DTC_DATABASE_HPP_

#include <chrono>
#include <unordered_map>

#include "mw/diag/dtc/database/dtc_database_element.hpp"
#include "core/common/error_code.h"
#include "core/logger/Logger.h"
namespace simba{
namespace mw{
namespace dtc{

class DtcDatabase{
 private:
 /**
  * @brief kluczem jest id błędu
  * 
  */
  std::unordered_map<uint16_t,DtcDatabaseElement> errors_{};
  core::logger::Logger logger_;
  uint16_t error_id;
  uint16_t active_errors;

 public:
  DtcDatabase();
  core::ErrorCode AddError(uint16_t dtc_error_code_,std::string details_,
                            uint8_t status_=0x0);
  core::ErrorCode ManResetError(uint16_t error_id);
  core::ErrorCode AutoResetError(uint16_t error_id);
  uint16_t ErrorNum();
  std::string GetErrorDetails(uint16_t error_id);

};

}  // dtc
}  // mw
}  // simba
#endif // _MW_DTC_DATABASE_HPP_