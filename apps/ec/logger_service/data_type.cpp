/**
 * @file data_type.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-22
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#include "apps/ec/logger_service/data_type.hpp"
#include <sstream>
#include <iomanip>
namespace srp {
namespace logger {

namespace {
  constexpr auto kCsv_header = "TIMESTAMP;TEMP1;TEMP2;TEMP3;TANK_PRESS;"
                  "TANK_D_PRESS;CPU_USAGE;MEM_USAGE;DISK_UTILIZATION";
}

std::string Data_t::get_header() {
  return kCsv_header;
}

std::vector<uint8_t> Data_t::get_bytes() {
  std::vector<uint8_t> bytes;
  std::unique_lock<std::mutex> lock(this->mutex_);

  auto append_bytes = [&bytes](auto& value) {
    const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&value);
    bytes.push_back(ptr);
  };
  append_bytes(temp1);
  append_bytes(temp2);
  append_bytes(temp3);
  append_bytes(tank_press);
  append_bytes(tank_d_press);
  append_bytes(sys_status.cpu_usage);
  append_bytes(sys_status.mem_usage);
  append_bytes(sys_status.disk_utilization);

  return bytes;
}

std::string Data_t::to_string(const std::string& timestamp) {
  std::stringstream res;
  res << std::fixed << std::setprecision(2);
  res << timestamp << ";";
  std::unique_lock<std::mutex> lock(this->mutex_);
  res << temp1 << ";";
  res << temp2 << ";";
  res << temp3 << ";";
  res << tank_press << ";";
  res << tank_d_press << ";";
  res << sys_status.cpu_usage << ";";
  res << sys_status.mem_usage << ";";
  res << sys_status.disk_utilization;
  return res.str();
}

void Data_t::SetSysStatus(const apps::SysStatType& sys_stat) {
  std::unique_lock<std::mutex> lock(this->mutex_);
  this->sys_status = sys_stat;
}

void Data_t::SetTemp1(const tempType& temp) {
  std::unique_lock<std::mutex> lock(this->mutex_);
  this->temp1 = temp;
}
void Data_t::SetTemp2(const tempType& temp) {
  std::unique_lock<std::mutex> lock(this->mutex_);
  this->temp2 = temp;
}
void Data_t::SetTemp3(const tempType& temp) {
  std::unique_lock<std::mutex> lock(this->mutex_);
  this->temp3 = temp;
}
void Data_t::SetTankPress(const pressType& press) {
  std::unique_lock<std::mutex> lock(this->mutex_);
  this->tank_press = press;
}
void Data_t::SetTankDPress(const dPressType& press) {
  std::unique_lock<std::mutex> lock(this->mutex_);
  this->tank_d_press = press;
}

}  // namespace logger
}  // namespace srp

