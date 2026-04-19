/**
 * @file data_type.cpp
 * @author Krzysztof Kondracki (kondracki.christopher@gmail.com)
 * @brief
 * @version 0.1
 * @date 2026-04-13
 *
 * @copyright Copyright (c) 2026
 *
 */
#include "apps/fc/logger_service/data_type.hpp"

#include <iomanip>
#include <sstream>

namespace srp {
namespace logger {

namespace {
constexpr auto kCsvHeader =
    "TIMESTAMP;BOARD_TEMP1;BOARD_TEMP2;BOARD_TEMP3;BME_TEMP;BME_HUMIDITY;"
    "BME_ALTITUDE;CPU_USAGE;MEM_USAGE;DISK_UTILIZATION";
}

std::string Data_t::get_header() {
  return kCsvHeader;
}

std::string Data_t::to_string(const std::string& timestamp) {
  std::stringstream res;
  res << std::fixed << std::setprecision(2);
  res << timestamp << ";";

  std::shared_lock<std::shared_mutex> lock(mutex_);
  res << board_temp1_ << ";";
  res << board_temp2_ << ";";
  res << board_temp3_ << ";";
  res << bme_temp_ << ";";
  res << bme_humidity_ << ";";
  res << bme_altitude_ << ";";
  res << cpu_usage_ << ";";
  res << mem_usage_ << ";";
  res << disk_utilization_;
  return res.str();
}

void Data_t::SetBoardTemp1(const tempType& temp) {
  std::unique_lock<std::shared_mutex> lock(mutex_);
  board_temp1_ = temp;
}

void Data_t::SetBoardTemp2(const tempType& temp) {
  std::unique_lock<std::shared_mutex> lock(mutex_);
  board_temp2_ = temp;
}

void Data_t::SetBoardTemp3(const tempType& temp) {
  std::unique_lock<std::shared_mutex> lock(mutex_);
  board_temp3_ = temp;
}

void Data_t::SetBmeTemp(const bmeType& value) {
  std::unique_lock<std::shared_mutex> lock(mutex_);
  bme_temp_ = value;
}

void Data_t::SetBmeHumidity(const bmeType& value) {
  std::unique_lock<std::shared_mutex> lock(mutex_);
  bme_humidity_ = value;
}

void Data_t::SetBmeAltitude(const bmeType& value) {
  std::unique_lock<std::shared_mutex> lock(mutex_);
  bme_altitude_ = value;
}

void Data_t::SetCpuUsage(const systemStatType& value) {
  std::unique_lock<std::shared_mutex> lock(mutex_);
  cpu_usage_ = value;
}

void Data_t::SetMemUsage(const systemStatType& value) {
  std::unique_lock<std::shared_mutex> lock(mutex_);
  mem_usage_ = value;
}

void Data_t::SetDiskUtilization(const systemStatType& value) {
  std::unique_lock<std::shared_mutex> lock(mutex_);
  disk_utilization_ = value;
}

}  // namespace logger
}  // namespace srp
