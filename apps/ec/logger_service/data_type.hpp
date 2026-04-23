/**
 * @file data_type.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-22
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef APPS_EC_LOGGER_SERVICE_DATA_TYPE_HPP_
#define APPS_EC_LOGGER_SERVICE_DATA_TYPE_HPP_

#include <shared_mutex>
#include <mutex>  // NOLINT
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
  std::shared_mutex mutex_;
  tensoType tenso;
  tempType temp1;
  tempType temp2;
  tempType temp3;
  pressType tank_press;
  dPressType tank_d_press;
  primerStatusType primer_status;
  servoType servo_status;
  servoType servo_dump_status;
  servoType servo_vent_status;
  engineType engine_mode;
  engineType engine_new_vent_valve_status;
  apps::SysStatType sys_status;

 public:
  std::string get_header();
  std::string to_string(const std::string& timestamp);
  std::vector<uint8_t> get_bytes(const int64_t& timestamp);
  void SetSysStatus(const apps::SysStatType& sys_stat);
  void SetTemp1(const tempType& temp);
  void SetTemp2(const tempType& temp);
  void SetTemp3(const tempType& temp);
  void SetTankPress(const pressType& press);
  void SetTankDPress(const dPressType& press);
  void SetTenso(const tensoType& tenso);
  void SetPrimerStatus(const primerStatusType& primer);
  void SetServoStatus(const servoType& status);
  void SetServoDumpStatus(const servoType& status);
  void SetServoVentStatus(const servoType& status);
  void SetEngineMode(const engineType& mode);
  void SetNewVentValveStatus(const engineType& status);
};

}  // namespace logger
}  // namespace srp

#endif  // APPS_EC_LOGGER_SERVICE_DATA_TYPE_HPP_
