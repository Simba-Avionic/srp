/**
 * @file TempDriver.cpp
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
     ara::log::LoggingMenager::GetInstance()->CreateLogger("temp", "", ara::log::LogLevel::kInfo);
}
ara::core::Result<double> TempDriver::ReadTemp(const std::string& sensorPhysicalID) {
    std::ifstream file(kSensorPath + sensorPhysicalID + "/temperature");
    if (!file) {
        ara::log::LogDebug() << "Sensor " << sensorPhysicalID << " not available!";
        return ara::com::MakeErrorCode(
            ara::com::ComErrc::kErroneousFileHandle, "Can't open file for 1Wire sensor");
    }
    std::string line;
    std::getline(file, line);
    try {
        const double value = static_cast<double>(std::stoi(line)) / 1000.0;
        ara::log::LogDebug() << "Read from ID: " << sensorPhysicalID << ", temp: " << std::to_string(value);
        return value;
    } catch (const std::invalid_argument& e) {
        ara::log::LogWarn() << "Invalid temperature data for sensor " << sensorPhysicalID;
        return ara::com::MakeErrorCode(
            ara::com::ComErrc::kErroneousFileHandle, "Invalid temperature data");
    }
}

ara::core::Result<bool> TempDriver::SetResolution(const std::string& sensorPhysicalID, uint8_t resolution) {
    std::ofstream file(kSensorPath + sensorPhysicalID + "/resolution");
    if (!file) {
        ara::log::LogWarn() << "Sensor " << sensorPhysicalID << " not available!";
        return ara::com::MakeErrorCode(
            ara::com::ComErrc::kErroneousFileHandle, "Can't open file for 1Wire sensor");
    }
    file << static_cast<int>(resolution);
    return true;
}

ara::core::Result<uint8_t> TempDriver::ReadResolution(const std::string& sensorPhysicalID) {
    std::ifstream file(kSensorPath + sensorPhysicalID + "/resolution");
    if (!file) {
        ara::log::LogWarn() << "Sensor " << sensorPhysicalID << " not available!";
        return ara::com::MakeErrorCode(
            ara::com::ComErrc::kErroneousFileHandle, "Can't open file for 1Wire sensor");
    }
    std::string line;
    std::getline(file, line);
    try {
        const uint8_t value = std::stoi(line);
        ara::log::LogDebug() << "Read from ID: " << sensorPhysicalID << ", resolution: " << value;
        return value;
    } catch (const std::invalid_argument& e) {
        ara::log::LogWarn() << "Invalid resolution data for sensor " << sensorPhysicalID;
        return ara::com::MakeErrorCode(
            ara::com::ComErrc::kErroneousFileHandle, "Invalid resolution data");
    }
}

ara::core::Result<std::vector<std::string>> TempDriver::GetAllAccessibleSensor() {
    std::vector<std::string> res;
    try {
        for (const auto& entry : std::filesystem::directory_iterator(kSensorPath)) {
            std::string name = entry.path().filename().string();
            if (entry.is_directory() && name != kManagerName) {
                res.push_back(name);
            }
        }
        return res;
    } catch (const std::filesystem::filesystem_error& e) {
        return ara::com::MakeErrorCode(
            ara::com::ComErrc::kErroneousFileHandle, "Can't search for 1Wire sensors");
    }
}
ara::core::Result<uint16_t> TempDriver::GetResponseTime(const std::string& sensorPhysicalID) {
    std::ifstream file(kSensorPath + sensorPhysicalID + "/conv_time");
    if (!file) {
        ara::log::LogWarn() << "Sensor " << sensorPhysicalID << " not available!";
        return ara::com::MakeErrorCode(
            ara::com::ComErrc::kErroneousFileHandle, "Can't open file for 1Wire sensor");
    }
    std::string line;
    std::getline(file, line);
    try {
        uint16_t value = std::stoi(line);
        ara::log::LogDebug() << "Read from ID: " << sensorPhysicalID << ", Resp Time: " << value;
        return value;
    } catch (const std::invalid_argument& e) {
        ara::log::LogWarn() << "Invalid Resp TIme data for sensor " << sensorPhysicalID;
        return ara::com::MakeErrorCode(
            ara::com::ComErrc::kErroneousFileHandle, "Invalid RESP data");
    }
}

}  // namespace temp
}  // namespace core
}  // namespace srp
