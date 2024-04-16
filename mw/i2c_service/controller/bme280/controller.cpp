/**
 * @file controller.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-04-12
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <unordered_map>
#include <string>
#include <vector>

#include "mw/i2c_service/controller/i2c_controller.h"
#include "mw/i2c_service/controller/bme280/controller.hpp"
namespace simba {
namespace i2c {

namespace {
    const constexpr uint8_t BME280_ADDRESS = 0x76;
}

void BME280::Init(uint16_t service_id) {
    this->i2c_.Init(service_id);
}
std::optional<double> BME280::GetTemperature() {
}
std::optional<double> BME280::GetHumidity() {
}
std::optional<double> BME280::GetPressure() {
}
}  // namespace i2c
}  // namespace simba

