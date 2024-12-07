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
namespace simba {
namespace logger {

std::string Data_t::to_string(const std::string& timestamp) {
  std::stringstream res;
  res << timestamp << ";";
  std::unique_lock<std::mutex> lock(this->mutex_);
  res << temp1 << ";";
  res << temp2 << ";";
  res << temp3 << ";";
  res << tank_press << ";";
  res << tank_d_press << ";";
  return res.str();
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
}  // namespace simba

