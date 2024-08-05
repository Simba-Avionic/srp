/**
 * @file Icsvdriver.h
 * @author Michał Mańkowski (m.mankowski2004@gmail.com)
 * @brief Interface for csvdriver
 * @version 0.1
 * @date 2024-08-05
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef APPS_LOGGER_SERVICE_CSVDRIVER_ICSVDRIVER_H_
#define APPS_LOGGER_SERVICE_CSVDRIVER_ICSVDRIVER_H_
#include <string>
#include <optional>
#include "core/common/error_code.h"
namespace simba {
namespace logger {

struct ActuatorData {
  std::optional<uint8_t>  main_valve;
  std::optional<uint8_t>  vent_valve;
  std::optional<uint8_t> primer;
};

struct SensorData {
  std::optional<float> nozle_press;
  std::optional<float> tank_press;
  std::optional<float> temp_1;
  std::optional<float> temp_2;
  std::optional<float> temp_3;
  std::optional<float> d_press;
};

class ICSVDriver {
 private:
  std::string fileName;
 public:
  virtual ~ICSVDriver() = default;
  virtual core::ErrorCode Init(const std::string& firstLine) = 0;
  virtual core::ErrorCode WriteLine(const ActuatorData& data, const double& timestamp) = 0;
  virtual core::ErrorCode WriteLine(const SensorData& data, const double& timestamp) = 0;
};
}  // namespace logger
}  // namespace simba
#endif  // APPS_LOGGER_SERVICE_CSVDRIVER_ICSVDRIVER_H_
