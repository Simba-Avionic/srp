/**
 * @file data_type.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.2
 * @date 2026-05-25
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
  using dPressType = float;
  using tensoType = float;
  using primerStatusType = uint8_t;
  using servoType = uint8_t;
  using engineType = uint8_t;

  std::atomic<tensoType> tenso{0.0f};
  std::atomic<tempType> temp1{0};
  std::atomic<tempType> temp2{0};
  std::atomic<tempType> temp3{0};
  std::atomic<tempType> board_temp1{0};
  std::atomic<tempType> board_temp2{0};
  std::atomic<tempType> board_temp3{0};
  std::atomic<pressType> tank_press{0.0f};
  std::atomic<dPressType> tank_d_press{0.0f};
  std::atomic<primerStatusType> primer_status{0};
  std::atomic<servoType> servo_status{0};
  std::atomic<servoType> servo_dump_status{0};
  std::atomic<servoType> servo_vent_status{0};
  std::atomic<engineType> engine_mode{0};
  std::atomic<engineType> engine_new_vent_valve_status{0};
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
  void SetTankPress(pressType press);
  void SetTankDPress(dPressType press);
  void SetTenso(tensoType tenso);
  void SetPrimerStatus(primerStatusType primer);
  void SetServoStatus(servoType status);
  void SetServoDumpStatus(servoType status);
  void SetServoVentStatus(servoType status);
  void SetEngineMode(engineType mode);
  void SetNewVentValveStatus(engineType status);
};

}  // namespace logger
}  // namespace srp

#endif  // APPS_EC_LOGGER_SERVICE_DATA_TYPE_HPP_
