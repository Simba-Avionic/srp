/**
 * @file sys_time.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-06-02
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef CORE_TIME_SYS_TIME_HPP_
#define CORE_TIME_SYS_TIME_HPP_

#include <cstdlib>
#include <string>
#include <optional>
#include "core/common/error_code.h"

namespace srp {
namespace core {
namespace time {

struct Date_t {
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t seconds;
    Date_t() {}
    Date_t(uint16_t year, uint8_t month, uint8_t day,
                                     uint8_t hour, uint8_t minute, uint8_t seconds):
            year(year), month(month), day(day), hour(hour), minute(minute), seconds(seconds) {}
};

class TimeChanger {
 protected:
  static std::string ConvertVariableToCommand(Date_t date);
  static std::optional<std::string> exec(const std::string& cmd);
  static Date_t ParseDate(const std::string& dateStr);
 public:
 /**
  * @brief 
  * 
  * @param time in format MMDDhhmmYYYY.ss
  * Example: "060210002024.00" sets the date to June 2, 2024, at 10:00:00 AM
  * @return core::ErrorCode 
  */
  static core::ErrorCode ChangeSystemTime(std::string time);
  static core::ErrorCode ChangeSystemTime(Date_t date);
  static std::optional<Date_t> ReadSystemTime();
  static std::optional<std::string> ReadSystemTimeAsString();
};
}  // namespace  time
}  // namespace  core
}  // namespace  srp

#endif  // CORE_TIME_SYS_TIME_HPP_
