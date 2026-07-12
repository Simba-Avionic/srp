/**
 * @file data_type.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.3
 * @date 2026-07-04
 * * @copyright Copyright (c) 2024-2026
 * */

#ifndef APPS_EC_LOGGER_SERVICE_DATA_TYPE_HPP_
#define APPS_EC_LOGGER_SERVICE_DATA_TYPE_HPP_

#include <atomic>
#include <cstdint>
#include <vector>
#include <memory>
#include <string>
#include "srp/apps/SysStatType.h"

namespace srp {
namespace logger {

class Data_t {
 private:
  using tempType = int16_t;
  using pressType = float;
  using valveType = uint8_t;
  using primerStatusType = uint8_t;
  using engineType = uint8_t;

  std::atomic<tempType> temp1{0};
  std::atomic<tempType> temp2{0};
  std::atomic<tempType> temp3{0};
  std::atomic<tempType> board_temp1{0};
  std::atomic<tempType> board_temp2{0};
  std::atomic<tempType> board_temp3{0};
  std::atomic<pressType> oxidizer_press{0.0f};
  std::atomic<pressType> pressure_feed_press{0.0f};
  std::atomic<pressType> chamber_press1{0.0f};
  std::atomic<primerStatusType> primer_status{0};
  std::atomic<valveType> oxidizer_main_valve{0};
  std::atomic<valveType> oxidizer_vent_valve{0};
  std::atomic<valveType> oxidizer_dump_valve{0};
  std::atomic<valveType> pressure_feed_main_valve{0};
  std::atomic<valveType> pressure_feed_vent_valve{0};
  std::atomic<engineType> engine_mode{0};
  std::atomic<pressType> ethanol_press{0.0f};
  std::atomic<pressType> chamber_press2{0.0f};
  std::atomic<pressType> chamber_press3{0.0f};
  std::atomic<tempType> sec_board_temp1{0};
  std::atomic<tempType> sec_board_temp2{0};
  std::atomic<tempType> sec_board_temp3{0};
  std::atomic<valveType> ethanol_main_valve{0};
  std::atomic<valveType> ethanol_vent_valve{0};
  std::atomic<float> sys_cpu_usage{0.0f};
  std::atomic<float> sys_mem_usage{0.0f};
  std::atomic<float> sys_disk_utilization{0.0f};
  std::atomic<uint32_t> gpio_states{0};

 public:
  Data_t() = default;

  Data_t(const Data_t&) = delete;
  Data_t& operator=(const Data_t&) = delete;

  std::string get_header();
  std::string to_string(const std::string& timestamp);
  std::vector<uint8_t> get_bytes(const int64_t& timestamp);

  void SetGpioState(const uint8_t pin_id, const uint8_t state);

  void SetSysStatus(const apps::SysStatType& sys_stat);
  void SetTemp1(tempType temp);
  void SetTemp2(tempType temp);
  void SetTemp3(tempType temp);
  void SetBoardTemp1(tempType temp);
  void SetBoardTemp2(tempType temp);
  void SetBoardTemp3(tempType temp);
  void SetOxidizerPress(pressType press);
  void SetPressureFeedPress(pressType press);
  void SetChamberPress1(pressType press);
  void SetPrimerStatus(primerStatusType primer);
  void SetOxidizerMainValve(valveType status);
  void SetOxidizerVentValve(valveType status);
  void SetOxidizerDumpValve(valveType status);
  void SetPressureFeedMainValve(valveType status);
  void SetPressureFeedVentValve(valveType status);
  void SetEngineMode(engineType mode);
  void SetEthanolPress(pressType press);
  void SetChamberPress2(pressType press);
  void SetChamberPress3(pressType press);
  void SetSecBoardTemp1(tempType temp);
  void SetSecBoardTemp2(tempType temp);
  void SetSecBoardTemp3(tempType temp);
  void SetEthanolMainValve(valveType status);
  void SetEthanolVentValve(valveType status);
};

}  // namespace logger
}  // namespace srp

#endif  // APPS_EC_LOGGER_SERVICE_DATA_TYPE_HPP_
