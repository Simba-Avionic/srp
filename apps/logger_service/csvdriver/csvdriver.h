/**
 * @file csvdriver.h
 * @author Michał Mańkowski (m.mankowski2004@gmail.com)
 * @brief Driver that operates on a logger service output file
 * @version 0.1
 * @date 2024-06-18
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef APPS_LOGGER_SERVICE_CSVDRIVER_CSVDRIVER_H_
#define APPS_LOGGER_SERVICE_CSVDRIVER_CSVDRIVER_H_
#include <vector>
#include <string>
#include <fstream>
#include "core/logger/Logger.h"
#include "apps/logger_service/csvdriver/Icsvdriver.h"
namespace simba {
namespace logger {
class CSVDriver : public ICSVDriver {
 private:
  std::string fileName;
 public:
  CSVDriver();
  explicit CSVDriver(const std::string& fileName) : fileName(fileName) {}
  core::ErrorCode Init(const std::string& firstLine) override;
  core::ErrorCode WriteLine(const ActuatorData& data, const double& timestamp) override;
  core::ErrorCode WriteLine(const SensorData& data, const double& timestamp) override;
};
}  // namespace logger
}  // namespace simba
#endif  // APPS_LOGGER_SERVICE_CSVDRIVER_CSVDRIVER_H_
