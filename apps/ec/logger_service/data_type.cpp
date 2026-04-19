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
  constexpr auto kCsv_header = "TIMESTAMP;TEMP1;TEMP2;TEMP3;BOARD_TEMP1;"
                  "BOARD_TEMP2;BOARD_TEMP3;TANK_PRESS;"
                  "TANK_D_PRESS;CPU_USAGE;MEM_USAGE;DISK_UTILIZATION;TENSO;"
                  "PRIMER_STATUS;SERVO_STATUS;SERVO_DUMP_STATUS;"
                  "SERVO_VENT_STATUS;ENGINE_MODE;ENGINE_NEW_VENT_VALVE_STATUS";
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
  tempType board_temp1_local;
  tempType board_temp2_local;
  tempType board_temp3_local;
  pressType tank_press_local;
  dPressType tank_d_press_local;
  apps::SysStatType sys_status_local;
  tensoType tenso_local;
  primerStatusType primer_status_local;
  servoType servo_status_local;
  servoType servo_dump_status_local;
  servoType servo_vent_status_local;
  engineType engine_mode_local;
  engineType engine_new_vent_valve_status_local;

  {
    std::unique_lock<std::shared_mutex> lock(this->mutex_);
    temp1_local = temp1;
    temp2_local = temp2;
    temp3_local = temp3;
    board_temp1_local = board_temp1;
    board_temp2_local = board_temp2;
    board_temp3_local = board_temp3;
    tank_press_local = tank_press;
    tank_d_press_local = tank_d_press;
    sys_status_local = sys_status;
    tenso_local = tenso;
    primer_status_local = primer_status;
    servo_status_local = servo_status;
    servo_dump_status_local = servo_dump_status;
    servo_vent_status_local = servo_vent_status;
    engine_mode_local = engine_mode;
    engine_new_vent_valve_status_local = engine_new_vent_valve_status;
  }

  constexpr std::size_t kTotalSize =
      sizeof(timestamp) +
      sizeof(temp1_local) +
      sizeof(temp2_local) +
      sizeof(temp3_local) +
      sizeof(board_temp1_local) +
      sizeof(board_temp2_local) +
      sizeof(board_temp3_local) +
      sizeof(tank_press_local) +
      sizeof(tank_d_press_local) +
      sizeof(sys_status_local.cpu_usage) +
      sizeof(sys_status_local.mem_usage) +
      sizeof(sys_status_local.disk_utilization) +
      sizeof(tenso_local) +
      sizeof(primer_status_local) +
      sizeof(servo_status_local) +
      sizeof(servo_dump_status_local) +
      sizeof(servo_vent_status_local) +
      sizeof(engine_mode_local) +
      sizeof(engine_new_vent_valve_status_local);

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
  append_bytes(offset, board_temp1_local);
  append_bytes(offset, board_temp2_local);
  append_bytes(offset, board_temp3_local);
  append_bytes(offset, tank_press_local);
  append_bytes(offset, tank_d_press_local);
  append_bytes(offset, sys_status_local.cpu_usage);
  append_bytes(offset, sys_status_local.mem_usage);
  append_bytes(offset, sys_status_local.disk_utilization);
  append_bytes(offset, tenso_local);
  append_bytes(offset, primer_status_local);
  append_bytes(offset, servo_status_local);
  append_bytes(offset, servo_dump_status_local);
  append_bytes(offset, servo_vent_status_local);
  append_bytes(offset, engine_mode_local);
  append_bytes(offset, engine_new_vent_valve_status_local);

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
  res << board_temp1 << ";";
  res << board_temp2 << ";";
  res << board_temp3 << ";";
  res << tank_press << ";";
  res << tank_d_press << ";";
  res << sys_status.cpu_usage << ";";
  res << sys_status.mem_usage << ";";
  res << sys_status.disk_utilization << ";";
  res << tenso << ";";
  res << static_cast<unsigned>(primer_status) << ";";
  res << static_cast<unsigned>(servo_status) << ";";
  res << static_cast<unsigned>(servo_dump_status) << ";";
  res << static_cast<unsigned>(servo_vent_status) << ";";
  res << static_cast<unsigned>(engine_mode) << ";";
  res << static_cast<unsigned>(engine_new_vent_valve_status);
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
void Data_t::SetBoardTemp1(const tempType& temp){
  std::unique_lock<std::shared_mutex> lock(this->mutex_);
  this->board_temp1 = temp;
}
void Data_t::SetBoardTemp2(const tempType& temp){
  std::unique_lock<std::shared_mutex> lock(this->mutex_);
  this->board_temp2 = temp;
}
void Data_t::SetBoardTemp3(const tempType& temp){
  std::unique_lock<std::shared_mutex> lock(this->mutex_);
  this->board_temp3 = temp;
}
void Data_t::SetTankPress(const pressType& press) {
  std::unique_lock<std::shared_mutex> lock(this->mutex_);
  this->tank_press = press;
}
void Data_t::SetTankDPress(const dPressType& press) {
  std::unique_lock<std::shared_mutex> lock(this->mutex_);
  this->tank_d_press = press;
}
void Data_t::SetPrimerStatus(const primerStatusType& primer) {
  std::unique_lock<std::shared_mutex> lock(this->mutex_);
  this->primer_status = primer;
}
void Data_t::SetServoStatus(const servoType& status) {
  std::unique_lock<std::shared_mutex> lock(this->mutex_);
  this->servo_status = status;
}
void Data_t::SetServoDumpStatus(const servoType& status) {
  std::unique_lock<std::shared_mutex> lock(this->mutex_);
  this->servo_dump_status = status;
}
void Data_t::SetServoVentStatus(const servoType& status) {
  std::unique_lock<std::shared_mutex> lock(this->mutex_);
  this->servo_vent_status = status;
}
void Data_t::SetEngineMode(const engineType& mode) {
  std::unique_lock<std::shared_mutex> lock(this->mutex_);
  this->engine_mode = mode;
}
void Data_t::SetNewVentValveStatus(const engineType& status) {
  std::unique_lock<std::shared_mutex> lock(this->mutex_);
  this->engine_new_vent_valve_status = status;
}

}  // namespace logger
}  // namespace srp
