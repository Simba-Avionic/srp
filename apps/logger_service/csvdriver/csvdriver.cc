/**
 * @file csvdriver.cc
 * @author Michał Mańkowski (m.mankowski2004@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-06-18
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "csvdriver.h"
#include <iostream>
namespace simba {
namespace logger {

core::ErrorCode CSVDriver::Init(const std::string& firstLine) {
    std::ofstream csvFile(this->fileName);
    if (!csvFile.is_open()) {
        AppLogger::Warning("Failed to open a file: " + fileName);
        std::cout << "Failed to open a file: " << fileName << "\n";
        return core::ErrorCode::kError;
    }
    csvFile << firstLine <<"\n";
    return core::ErrorCode::kOk;
}

core::ErrorCode CSVDriver::WriteLine(const ActuatorData& data, const double& timestamp) {
    std::ofstream csvFile(this->fileName, std::ios::app);
    if (!csvFile.is_open()) {
        AppLogger::Warning("Failed to open a file: " + fileName);
        return core::ErrorCode::kError;
    }
    csvFile << timestamp << ";";
    std::vector<std::optional<uint8_t>> actuators = {data.main_valve, data.vent_valve, data.primer};
    for (const auto& actuator : actuators) {
        if (actuator.has_value()) {
            csvFile << static_cast<int>(actuator.value());
        }
        csvFile << ";";
    }
    csvFile << "\n";
    csvFile.close();
    return core::ErrorCode::kOk;
}

  core::ErrorCode CSVDriver::WriteLine(const SensorData& data, const double& timestamp) {
    std::ofstream csvFile(this->fileName, std::ios::app);
    if (!csvFile.is_open()) {
        AppLogger::Warning("Failed to open a file: " + fileName);
        return core::ErrorCode::kError;
    }
    csvFile << timestamp << ";";
    std::vector<std::optional<float>> sensors = {data.nozle_press, data.tank_press, data.temp_1,
        data.temp_2, data.temp_3, data.d_press};
    for (const auto& sensor : sensors) {
        if (sensor.has_value()) {
            csvFile << sensor.value();
        }
        csvFile << ";";
    }
    csvFile << "\n";
    csvFile.close();
    return core::ErrorCode::kOk;
  }
}  // namespace logger
}  // namespace simba
