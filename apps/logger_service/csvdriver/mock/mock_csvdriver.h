/**
 * @file mock_csvdriver.h
 * @author Michał Mańkowski (m.mankowski2004@gmail.com)
 * @brief mock csvdriver
 * @version 0.1
 * @date 2024-08-05
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef APPS_LOGGER_SERVICE_CSVDRIVER_MOCK_MOCK_CSVDRIVER_H_
#define APPS_LOGGER_SERVICE_CSVDRIVER_MOCK_MOCK_CSVDRIVER_H_
#include <string>
#include "gmock/gmock.h"
#include "apps/logger_service/csvdriver/Icsvdriver.h"
namespace simba {
namespace mock {
class MOCKCSVDRIVER : public logger::ICSVDriver {
 public:
  MOCK_METHOD(core::ErrorCode, Init, (const std::string& firstLine, const char& separator), (override));
  MOCK_METHOD(core::ErrorCode, WriteLine, (const logger::ActuatorData& data, const double& timestamp), (override));
  MOCK_METHOD(core::ErrorCode, WriteLine, (const logger::SensorData& data, const double& timestamp), (override));
};
}   // namespace mock
}   // namespace simba

#endif  // APPS_LOGGER_SERVICE_CSVDRIVER_MOCK_MOCK_CSVDRIVER_H_
