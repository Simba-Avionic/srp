/**
 * @file SIMTempDriver.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-07
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <optional>
#include <vector>
#include <cstdint>
#include <filesystem>
#include <string>
#include <fstream>
#include <stdexcept>
#include "ara/log/log.h"
#include "ara/core/result.h"
#include "core/temp/temp_driver.hpp"
#include "core/common/condition.h"
#include "ara/com/com_error_domain.h"

namespace srp {
namespace core {
namespace temp {

namespace {
    constexpr auto kSensorPath = "/sys/bus/w1/devices/";
    constexpr auto kManagerName = "w1_bus_master1";
}
TempDriver::TempDriver() {
     ara::log::LoggingMenager::GetInstance()->CreateLogger("Dtem", "", ara::log::LogLevel::kInfo);
}
ara::core::Result<double> TempDriver::ReadTemp(const std::string& sensorPhysicalID) {
    ara::log::LogInfo() << "Receive Reading REQ from sensor " << sensorPhysicalID;
    return 33.33;
}

ara::core::Result<bool> TempDriver::SetResolution(const std::string& sensorPhysicalID, uint8_t resolution) {
    ara::log::LogInfo() << "Receive SET Resolution REQ on sensor " << sensorPhysicalID << ", to res: " << resolution;
    return true;
}

ara::core::Result<uint8_t> TempDriver::ReadResolution(const std::string& sensorPhysicalID) {
    ara::log::LogInfo() << "Receive Reading RES REQ from sensor " << sensorPhysicalID;
    return 9;
}

ara::core::Result<std::vector<std::string>> TempDriver::GetAllAccessibleSensor() {
    std::vector<std::string> res;
    ara::log::LogInfo() << "Receive Reading ALL SENSORS ";
    return res;
}
ara::core::Result<uint16_t> TempDriver::GetResponseTime(const std::string& sensorPhysicalID) {
    ara::log::LogInfo() << "Receive Reading RES time REQ from sensor: " << sensorPhysicalID;
    return 250;
}

}  // namespace temp
}  // namespace core
}  // namespace srp
