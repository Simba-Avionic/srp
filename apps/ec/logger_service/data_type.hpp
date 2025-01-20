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
#include <memory>
#include <string>

namespace srp {
namespace logger {

class Data_t {
 private:
  using tempType = int16_t;
  using pressType = float;
  using dPressType = float;
  std::mutex mutex_;
  tempType temp1;
  tempType temp2;
  tempType temp3;
  pressType tank_press;
  dPressType tank_d_press;
 public:
  std::string to_string(const std::string& timestamp);
  void SetTemp1(const tempType& temp);
  void SetTemp2(const tempType& temp);
  void SetTemp3(const tempType& temp);
  void SetTankPress(const pressType& press);
  void SetTankDPress(const dPressType& press);
};

}  // namespace logger
}  // namespace srp

#endif  // APPS_EC_LOGGER_SERVICE_DATA_TYPE_HPP_
