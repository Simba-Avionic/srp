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
#include <cstring>
namespace srp {
namespace logger {

namespace {
  constexpr auto kCsv_header = "TIMESTAMP;TEMP1;TEMP2;TEMP3;TANK_PRESS;"
                  "TANK_D_PRESS;CPU_USAGE;MEM_USAGE;DISK_UTILIZATION;TENSO";
}

void Data_t::SetTenso(const tensoType& tenso) {
  std::unique_lock<std::shared_mutex> lock(this->mutex_);
  this->tenso = tenso;
}

std::string Data_t::get_header() {
  return kCsv_header;
}

std::vector<uint8_t> Data_t::get_bytes(const int64_t& timestamp) {
  tempType temp1_local;
  tempType temp2_local;
  tempType temp3_local;
  pressType tank_press_local;
  dPressType tank_d_press_local;
  apps::SysStatType sys_status_local;

  {
    std::unique_lock<std::shared_mutex> lock(this->mutex_);
    temp1_local = temp1;
    temp2_local = temp2;
    temp3_local = temp3;
    tank_press_local = tank_press;
    tank_d_press_local = tank_d_press;
    sys_status_local = sys_status;
  }

  constexpr std::size_t kTotalSize =
      sizeof(timestamp) +
      sizeof(temp1_local) +
      sizeof(temp2_local) +
      sizeof(temp3_local) +
      sizeof(tank_press_local) +
      sizeof(tank_d_press_local) +
      sizeof(sys_status_local.cpu_usage) +
      sizeof(sys_status_local.mem_usage) +
      sizeof(sys_status_local.disk_utilization);

  std::vector<uint8_t> bytes;
  bytes.resize(kTotalSize);

  auto append_bytes = [&bytes](std::size_t& offset, const auto& value) {
    const std::size_t size = sizeof(value);
    const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&value);
    std::memcpy(bytes.data() + offset, ptr, size);
    offset += size;
  };

  std::size_t offset = 0U;
  append_bytes(offset, timestamp);
  append_bytes(offset, temp1_local);
  append_bytes(offset, temp2_local);
  append_bytes(offset, temp3_local);
  append_bytes(offset, tank_press_local);
  append_bytes(offset, tank_d_press_local);
  append_bytes(offset, sys_status_local.cpu_usage);
  append_bytes(offset, sys_status_local.mem_usage);
  append_bytes(offset, sys_status_local.disk_utilization);

  return bytes;
}

std::string Data_t::to_string(const std::string& timestamp) {
  std::stringstream res;
  res << std::fixed << std::setprecision(2);
  res << timestamp << ";";
  std::shared_lock<std::shared_mutex> lock(this->mutex_);
  res << temp1 << ";";
  res << temp2 << ";";
  res << temp3 << ";";
  res << tank_press << ";";
  res << tank_d_press << ";";
  res << sys_status.cpu_usage << ";";
  res << sys_status.mem_usage << ";";
  res << sys_status.disk_utilization << ";";
  res << tenso;
  return res.str();
}

void Data_t::SetSysStatus(const apps::SysStatType& sys_stat) {
  std::unique_lock<std::shared_mutex> lock(this->mutex_);
  this->sys_status = sys_stat;
}

void Data_t::SetTemp1(const tempType& temp) {
  std::unique_lock<std::shared_mutex> lock(this->mutex_);
  this->temp1 = temp;
}
void Data_t::SetTemp2(const tempType& temp) {
  std::unique_lock<std::shared_mutex> lock(this->mutex_);
  this->temp2 = temp;
}
void Data_t::SetTemp3(const tempType& temp) {
  std::unique_lock<std::shared_mutex> lock(this->mutex_);
  this->temp3 = temp;
}
void Data_t::SetTankPress(const pressType& press) {
  std::unique_lock<std::shared_mutex> lock(this->mutex_);
  this->tank_press = press;
}
void Data_t::SetTankDPress(const dPressType& press) {
  std::unique_lock<std::shared_mutex> lock(this->mutex_);
  this->tank_d_press = press;
}

}  // namespace logger
}  // namespace srp

