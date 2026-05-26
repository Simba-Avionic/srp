/**
 * @file data_type.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.2
 * @date 2026-05-25
 * * @copyright Copyright (c) 2024-2026
 * */

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

std::string Data_t::get_header() {
  return kCsv_header;
}

std::vector<uint8_t> Data_t::get_bytes(const int64_t& timestamp) {
  // Pobieramy wszystkie wartości lokalnie na starcie metodą relaxed
  const tempType temp1_local = temp1.load(std::memory_order_relaxed);
  const tempType temp2_local = temp2.load(std::memory_order_relaxed);
  const tempType temp3_local = temp3.load(std::memory_order_relaxed);
  const tempType board_temp1_local = board_temp1.load(std::memory_order_relaxed);
  const tempType board_temp2_local = board_temp2.load(std::memory_order_relaxed);
  const tempType board_temp3_local = board_temp3.load(std::memory_order_relaxed);
  const pressType tank_press_local = tank_press.load(std::memory_order_relaxed);
  const dPressType tank_d_press_local = tank_d_press.load(std::memory_order_relaxed);
  
  // Odczyt rozbitych pól systemowych
  const auto cpu_local = sys_cpu_usage.load(std::memory_order_relaxed);
  const auto mem_local = sys_mem_usage.load(std::memory_order_relaxed);
  const auto disk_local = sys_disk_utilization.load(std::memory_order_relaxed);

  const tensoType tenso_local = tenso.load(std::memory_order_relaxed);
  const primerStatusType primer_status_local = primer_status.load(std::memory_order_relaxed);
  const servoType servo_status_local = servo_status.load(std::memory_order_relaxed);
  const servoType servo_dump_status_local = servo_dump_status.load(std::memory_order_relaxed);
  const servoType servo_vent_status_local = servo_vent_status.load(std::memory_order_relaxed);
  const engineType engine_mode_local = engine_mode.load(std::memory_order_relaxed);
  const engineType engine_new_vent_valve_status_local = engine_new_vent_valve_status.load(std::memory_order_relaxed);

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
      sizeof(cpu_local) +
      sizeof(mem_local) +
      sizeof(disk_local) +
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
  append_bytes(offset, cpu_local);
  append_bytes(offset, mem_local);
  append_bytes(offset, disk_local);
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
  // Pobieramy lokalne kopie wszystkich zmiennych na początku funkcji
  const tempType temp1_local = temp1.load(std::memory_order_relaxed);
  const tempType temp2_local = temp2.load(std::memory_order_relaxed);
  const tempType temp3_local = temp3.load(std::memory_order_relaxed);
  const tempType board_temp1_local = board_temp1.load(std::memory_order_relaxed);
  const tempType board_temp2_local = board_temp2.load(std::memory_order_relaxed);
  const tempType board_temp3_local = board_temp3.load(std::memory_order_relaxed);
  const pressType tank_press_local = tank_press.load(std::memory_order_relaxed);
  const dPressType tank_d_press_local = tank_d_press.load(std::memory_order_relaxed);
  
  const auto cpu_local = sys_cpu_usage.load(std::memory_order_relaxed);
  const auto mem_local = sys_mem_usage.load(std::memory_order_relaxed);
  const auto disk_local = sys_disk_utilization.load(std::memory_order_relaxed);

  const tensoType tenso_local = tenso.load(std::memory_order_relaxed);
  const unsigned primer_local = static_cast<unsigned>(primer_status.load(std::memory_order_relaxed));
  const unsigned servo_local = static_cast<unsigned>(servo_status.load(std::memory_order_relaxed));
  const unsigned servo_dump_local = static_cast<unsigned>(servo_dump_status.load(std::memory_order_relaxed));
  const unsigned servo_vent_local = static_cast<unsigned>(servo_vent_status.load(std::memory_order_relaxed));
  const unsigned engine_mode_local = static_cast<unsigned>(engine_mode.load(std::memory_order_relaxed));
  const unsigned engine_vent_valve_local = static_cast<unsigned>(engine_new_vent_valve_status.load(std::memory_order_relaxed));

  std::stringstream res;
  res << std::fixed << std::setprecision(2);
  res << timestamp << ";";
  res << temp1_local << ";";
  res << temp2_local << ";";
  res << temp3_local << ";";
  res << board_temp1_local << ";";
  res << board_temp2_local << ";";
  res << board_temp3_local << ";";
  res << tank_press_local << ";";
  res << tank_d_press_local << ";";
  res << cpu_local << ";";
  res << mem_local << ";";
  res << disk_local << ";";
  res << tenso_local << ";";
  res << primer_local << ";";
  res << servo_local << ";";
  res << servo_dump_local << ";";
  res << servo_vent_local << ";";
  res << engine_mode_local << ";";
  res << engine_vent_valve_local;
  return res.str();
}

void Data_t::SetSysStatus(const apps::SysStatType& sys_stat) {
  this->sys_cpu_usage.store(sys_stat.cpu_usage, std::memory_order_relaxed);
  this->sys_mem_usage.store(sys_stat.mem_usage, std::memory_order_relaxed);
  this->sys_disk_utilization.store(sys_stat.disk_utilization, std::memory_order_relaxed);
}

void Data_t::SetTenso(tensoType tenso) {
  this->tenso.store(tenso, std::memory_order_relaxed);
}

void Data_t::SetTemp1(tempType temp) {
  this->temp1.store(temp, std::memory_order_relaxed);
}

void Data_t::SetTemp2(tempType temp) {
  this->temp2.store(temp, std::memory_order_relaxed);
}

void Data_t::SetTemp3(tempType temp) {
  this->temp3.store(temp, std::memory_order_relaxed);
}

void Data_t::SetBoardTemp1(tempType temp) {
  this->board_temp1.store(temp, std::memory_order_relaxed);
}

void Data_t::SetBoardTemp2(tempType temp) {
  this->board_temp2.store(temp, std::memory_order_relaxed);
}

void Data_t::SetBoardTemp3(tempType temp) {
  this->board_temp3.store(temp, std::memory_order_relaxed);
}

void Data_t::SetTankPress(pressType press) {
  this->tank_press.store(press, std::memory_order_relaxed);
}

void Data_t::SetTankDPress(dPressType press) {
  this->tank_d_press.store(press, std::memory_order_relaxed);
}

void Data_t::SetPrimerStatus(primerStatusType primer) {
  this->primer_status.store(primer, std::memory_order_relaxed);
}

void Data_t::SetServoStatus(servoType status) {
  this->servo_status.store(status, std::memory_order_relaxed);
}

void Data_t::SetServoDumpStatus(servoType status) {
  this->servo_dump_status.store(status, std::memory_order_relaxed);
}

void Data_t::SetServoVentStatus(servoType status) {
  this->servo_vent_status.store(status, std::memory_order_relaxed);
}

void Data_t::SetEngineMode(engineType mode) {
  this->engine_mode.store(mode, std::memory_order_relaxed);
}

void Data_t::SetNewVentValveStatus(engineType status) {
  this->engine_new_vent_valve_status.store(status, std::memory_order_relaxed);
}

}  // namespace logger
}  // namespace srp