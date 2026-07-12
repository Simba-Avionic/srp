/**
 * @file data_type.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.3
 * @date 2026-07-04
 * * @copyright Copyright (c) 2024-2026
 * */

#include "apps/ec/logger_service/data_type.hpp"
#include <sstream>
#include <iomanip>
#include <cstring>
#include "ara/log/log.h"

namespace srp {
namespace logger {

namespace {
  constexpr auto kCsv_header =
      "TIMESTAMP;TEMP1;TEMP2;TEMP3;BOARD_TEMP1;BOARD_TEMP2;BOARD_TEMP3;"
      "OXIDIZER_PRESS;PRESSURE_FEED_PRESS;CHAMBER_PRESS1;"
      "CPU_USAGE;MEM_USAGE;DISK_UTILIZATION;"
      "PRIMER_STATUS;"
      "OXIDIZER_MAIN_VALVE;OXIDIZER_VENT_VALVE;OXIDIZER_DUMP_VALVE;"
      "PRESSURE_FEED_MAIN_VALVE;PRESSURE_FEED_VENT_VALVE;"
      "ENGINE_MODE;"
      "ETHANOL_PRESS;CHAMBER_PRESS2;CHAMBER_PRESS3;"
      "SEC_BOARD_TEMP1;SEC_BOARD_TEMP2;SEC_BOARD_TEMP3;"
      "ETHANOL_MAIN_VALVE;ETHANOL_VENT_VALVE;GPIO_STATE";
}  // namespace

std::string Data_t::get_header() {
  return kCsv_header;
}

std::vector<uint8_t> Data_t::get_bytes(const int64_t& timestamp) {
  const tempType temp1_local = temp1.load(std::memory_order_relaxed);
  const tempType temp2_local = temp2.load(std::memory_order_relaxed);
  const tempType temp3_local = temp3.load(std::memory_order_relaxed);
  const tempType board_temp1_local = board_temp1.load(std::memory_order_relaxed);
  const tempType board_temp2_local = board_temp2.load(std::memory_order_relaxed);
  const tempType board_temp3_local = board_temp3.load(std::memory_order_relaxed);
  const pressType oxidizer_press_local = oxidizer_press.load(std::memory_order_relaxed);
  const pressType pressure_feed_press_local = pressure_feed_press.load(std::memory_order_relaxed);
  const pressType chamber_press1_local = chamber_press1.load(std::memory_order_relaxed);

  const auto cpu_local = sys_cpu_usage.load(std::memory_order_relaxed);
  const auto mem_local = sys_mem_usage.load(std::memory_order_relaxed);
  const auto disk_local = sys_disk_utilization.load(std::memory_order_relaxed);

  const primerStatusType primer_status_local = primer_status.load(std::memory_order_relaxed);
  const valveType oxidizer_main_valve_local = oxidizer_main_valve.load(std::memory_order_relaxed);
  const valveType oxidizer_vent_valve_local = oxidizer_vent_valve.load(std::memory_order_relaxed);
  const valveType oxidizer_dump_valve_local = oxidizer_dump_valve.load(std::memory_order_relaxed);
  const valveType pressure_feed_main_valve_local =
      pressure_feed_main_valve.load(std::memory_order_relaxed);
  const valveType pressure_feed_vent_valve_local =
      pressure_feed_vent_valve.load(std::memory_order_relaxed);
  const engineType engine_mode_local = engine_mode.load(std::memory_order_relaxed);
  const pressType ethanol_press_local = ethanol_press.load(std::memory_order_relaxed);
  const pressType chamber_press2_local = chamber_press2.load(std::memory_order_relaxed);
  const pressType chamber_press3_local = chamber_press3.load(std::memory_order_relaxed);
  const tempType sec_board_temp1_local = sec_board_temp1.load(std::memory_order_relaxed);
  const tempType sec_board_temp2_local = sec_board_temp2.load(std::memory_order_relaxed);
  const tempType sec_board_temp3_local = sec_board_temp3.load(std::memory_order_relaxed);
  const valveType ethanol_main_valve_local = ethanol_main_valve.load(std::memory_order_relaxed);
  const valveType ethanol_vent_valve_local = ethanol_vent_valve.load(std::memory_order_relaxed);

  const uint32_t gpio_ = gpio_states.load(std::memory_order_relaxed);

  constexpr std::size_t kTotalSize =
      sizeof(timestamp) +
      sizeof(temp1_local) +
      sizeof(temp2_local) +
      sizeof(temp3_local) +
      sizeof(board_temp1_local) +
      sizeof(board_temp2_local) +
      sizeof(board_temp3_local) +
      sizeof(oxidizer_press_local) +
      sizeof(pressure_feed_press_local) +
      sizeof(chamber_press1_local) +
      sizeof(cpu_local) +
      sizeof(mem_local) +
      sizeof(disk_local) +
      sizeof(primer_status_local) +
      sizeof(oxidizer_main_valve_local) +
      sizeof(oxidizer_vent_valve_local) +
      sizeof(oxidizer_dump_valve_local) +
      sizeof(pressure_feed_main_valve_local) +
      sizeof(pressure_feed_vent_valve_local) +
      sizeof(engine_mode_local) +
      sizeof(ethanol_press_local) +
      sizeof(chamber_press2_local) +
      sizeof(chamber_press3_local) +
      sizeof(sec_board_temp1_local) +
      sizeof(sec_board_temp2_local) +
      sizeof(sec_board_temp3_local) +
      sizeof(ethanol_main_valve_local) +
      sizeof(ethanol_vent_valve_local) +
      sizeof(gpio_);

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
  append_bytes(offset, oxidizer_press_local);
  append_bytes(offset, pressure_feed_press_local);
  append_bytes(offset, chamber_press1_local);
  append_bytes(offset, cpu_local);
  append_bytes(offset, mem_local);
  append_bytes(offset, disk_local);
  append_bytes(offset, primer_status_local);
  append_bytes(offset, oxidizer_main_valve_local);
  append_bytes(offset, oxidizer_vent_valve_local);
  append_bytes(offset, oxidizer_dump_valve_local);
  append_bytes(offset, pressure_feed_main_valve_local);
  append_bytes(offset, pressure_feed_vent_valve_local);
  append_bytes(offset, engine_mode_local);
  append_bytes(offset, ethanol_press_local);
  append_bytes(offset, chamber_press2_local);
  append_bytes(offset, chamber_press3_local);
  append_bytes(offset, sec_board_temp1_local);
  append_bytes(offset, sec_board_temp2_local);
  append_bytes(offset, sec_board_temp3_local);
  append_bytes(offset, ethanol_main_valve_local);
  append_bytes(offset, ethanol_vent_valve_local);
  append_bytes(offset, gpio_);

  return bytes;
}

std::string Data_t::to_string(const std::string& timestamp) {
  const tempType temp1_local = temp1.load(std::memory_order_relaxed);
  const tempType temp2_local = temp2.load(std::memory_order_relaxed);
  const tempType temp3_local = temp3.load(std::memory_order_relaxed);
  const tempType board_temp1_local = board_temp1.load(std::memory_order_relaxed);
  const tempType board_temp2_local = board_temp2.load(std::memory_order_relaxed);
  const tempType board_temp3_local = board_temp3.load(std::memory_order_relaxed);
  const pressType oxidizer_press_local = oxidizer_press.load(std::memory_order_relaxed);
  const pressType pressure_feed_press_local = pressure_feed_press.load(std::memory_order_relaxed);
  const pressType chamber_press1_local = chamber_press1.load(std::memory_order_relaxed);

  const auto cpu_local = sys_cpu_usage.load(std::memory_order_relaxed);
  const auto mem_local = sys_mem_usage.load(std::memory_order_relaxed);
  const auto disk_local = sys_disk_utilization.load(std::memory_order_relaxed);

  const auto primer_local = static_cast<int>(primer_status.load(std::memory_order_relaxed));
  const auto oxidizer_main_local =
      static_cast<int>(oxidizer_main_valve.load(std::memory_order_relaxed));
  const auto oxidizer_vent_local =
      static_cast<int>(oxidizer_vent_valve.load(std::memory_order_relaxed));
  const auto oxidizer_dump_local =
      static_cast<int>(oxidizer_dump_valve.load(std::memory_order_relaxed));
  const auto pressure_feed_main_local =
      static_cast<int>(pressure_feed_main_valve.load(std::memory_order_relaxed));
  const auto pressure_feed_vent_local =
      static_cast<int>(pressure_feed_vent_valve.load(std::memory_order_relaxed));
  const auto engine_mode_local = static_cast<int>(engine_mode.load(std::memory_order_relaxed));
  const pressType ethanol_press_local = ethanol_press.load(std::memory_order_relaxed);
  const pressType chamber_press2_local = chamber_press2.load(std::memory_order_relaxed);
  const pressType chamber_press3_local = chamber_press3.load(std::memory_order_relaxed);
  const tempType sec_board_temp1_local = sec_board_temp1.load(std::memory_order_relaxed);
  const tempType sec_board_temp2_local = sec_board_temp2.load(std::memory_order_relaxed);
  const tempType sec_board_temp3_local = sec_board_temp3.load(std::memory_order_relaxed);
  const auto ethanol_main_local =
      static_cast<int>(ethanol_main_valve.load(std::memory_order_relaxed));
  const auto ethanol_vent_local =
      static_cast<int>(ethanol_vent_valve.load(std::memory_order_relaxed));

  const uint32_t gpio_ = gpio_states.load(std::memory_order_relaxed);

  std::stringstream res;
  res << std::fixed << std::setprecision(4);
  res << timestamp << ";";
  res << temp1_local << ";";
  res << temp2_local << ";";
  res << temp3_local << ";";
  res << board_temp1_local << ";";
  res << board_temp2_local << ";";
  res << board_temp3_local << ";";
  res << oxidizer_press_local << ";";
  res << pressure_feed_press_local << ";";
  res << chamber_press1_local << ";";
  res << cpu_local << ";";
  res << mem_local << ";";
  res << disk_local << ";";
  res << primer_local << ";";
  res << oxidizer_main_local << ";";
  res << oxidizer_vent_local << ";";
  res << oxidizer_dump_local << ";";
  res << pressure_feed_main_local << ";";
  res << pressure_feed_vent_local << ";";
  res << engine_mode_local << ";";
  res << ethanol_press_local << ";";
  res << chamber_press2_local << ";";
  res << chamber_press3_local << ";";
  res << sec_board_temp1_local << ";";
  res << sec_board_temp2_local << ";";
  res << sec_board_temp3_local << ";";
  res << ethanol_main_local << ";";
  res << ethanol_vent_local << ";";
  res << gpio_;
  return res.str();
}

void Data_t::SetSysStatus(const apps::SysStatType& sys_stat) {
  this->sys_cpu_usage.store(sys_stat.cpu_usage, std::memory_order_relaxed);
  this->sys_mem_usage.store(sys_stat.mem_usage, std::memory_order_relaxed);
  this->sys_disk_utilization.store(sys_stat.disk_utilization, std::memory_order_relaxed);
}

void Data_t::SetGpioState(const uint8_t pin_id, const uint8_t state) {
  if (pin_id >= 32) {
    ara::log::LogWarn() << "GPIO pin_id out of range: " << static_cast<int>(pin_id);
    return;
  }
  const uint32_t mask = 1U << pin_id;
  if (state == 0) {
    gpio_states.fetch_and(~mask, std::memory_order_relaxed);
  } else {
    gpio_states.fetch_or(mask, std::memory_order_relaxed);
  }
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

void Data_t::SetOxidizerPress(pressType press) {
  this->oxidizer_press.store(press, std::memory_order_relaxed);
}

void Data_t::SetPressureFeedPress(pressType press) {
  this->pressure_feed_press.store(press, std::memory_order_relaxed);
}

void Data_t::SetChamberPress1(pressType press) {
  this->chamber_press1.store(press, std::memory_order_relaxed);
}

void Data_t::SetPrimerStatus(primerStatusType primer) {
  this->primer_status.store(primer, std::memory_order_relaxed);
}

void Data_t::SetOxidizerMainValve(valveType status) {
  this->oxidizer_main_valve.store(status, std::memory_order_relaxed);
}

void Data_t::SetOxidizerVentValve(valveType status) {
  this->oxidizer_vent_valve.store(status, std::memory_order_relaxed);
}

void Data_t::SetOxidizerDumpValve(valveType status) {
  this->oxidizer_dump_valve.store(status, std::memory_order_relaxed);
}

void Data_t::SetPressureFeedMainValve(valveType status) {
  this->pressure_feed_main_valve.store(status, std::memory_order_relaxed);
}

void Data_t::SetPressureFeedVentValve(valveType status) {
  this->pressure_feed_vent_valve.store(status, std::memory_order_relaxed);
}

void Data_t::SetEngineMode(engineType mode) {
  this->engine_mode.store(mode, std::memory_order_relaxed);
}

void Data_t::SetEthanolPress(pressType press) {
  this->ethanol_press.store(press, std::memory_order_relaxed);
}

void Data_t::SetChamberPress2(pressType press) {
  this->chamber_press2.store(press, std::memory_order_relaxed);
}

void Data_t::SetChamberPress3(pressType press) {
  this->chamber_press3.store(press, std::memory_order_relaxed);
}

void Data_t::SetSecBoardTemp1(tempType temp) {
  this->sec_board_temp1.store(temp, std::memory_order_relaxed);
}

void Data_t::SetSecBoardTemp2(tempType temp) {
  this->sec_board_temp2.store(temp, std::memory_order_relaxed);
}

void Data_t::SetSecBoardTemp3(tempType temp) {
  this->sec_board_temp3.store(temp, std::memory_order_relaxed);
}

void Data_t::SetEthanolMainValve(valveType status) {
  this->ethanol_main_valve.store(status, std::memory_order_relaxed);
}

void Data_t::SetEthanolVentValve(valveType status) {
  this->ethanol_vent_valve.store(status, std::memory_order_relaxed);
}

}  // namespace logger
}  // namespace srp
