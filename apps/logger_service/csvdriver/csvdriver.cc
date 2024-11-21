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

core::ErrorCode CSVDriver::Init(const char& separator) {
    this->separator = separator;
    std::ofstream csvFile(this->fileName);
    if (!csvFile.is_open()) {
        AppLogger::Warning("Failed to open a file: " + fileName);
        std::cout << "Failed to open a file: " << fileName << "\n";
        return core::ErrorCode::kError;
    }
    csvFile << "TIMESTAMP";
    if (this->main_valve) {
        csvFile << ";MAIN_VALVE";
    }
    if (this->vent_valve) {
        csvFile << ";VENT_VALVE";
    }
    if (this->primer) {
        csvFile << ";PRIMER";
    }
    if (this->nozle_press) {
        csvFile << ";NOZLE_PRESS";
    }
    if (this->tank_press) {
        csvFile << ";TANK_PRESS";
    }
    if (this->temp_1) {
        csvFile << ";TANK_TEMP_1";
    }
    if (this->temp_2) {
        csvFile << ";TANK_TEMP_2";
    }
    if (this->temp_3) {
        csvFile << ";TANK_TEMP_3";
    }
    if (this->d_press) {
        csvFile << ";TANK_D_PRESS";
    }
    return core::ErrorCode::kOk;
}

core::ErrorCode CSVDriver::WriteLine(const ActuatorData& data, const uint32_t& timestamp) {
    std::ofstream csvFile(this->fileName, std::ios::app);
    if (!csvFile.is_open()) {
        AppLogger::Warning("Failed to open a file: " + fileName);
        return core::ErrorCode::kError;
    }
    csvFile << timestamp << this->separator;
    std::vector<std::optional<uint8_t>> actuators;
    if (this->main_valve) {
        actuators.push_back(data.main_valve);
    }
    if (this->vent_valve) {
        actuators.push_back(data.vent_valve);
    }
    if (this->primer) {
        actuators.push_back(data.primer);
    }
    for (const auto& actuator : actuators) {
        if (actuator.has_value()) {
            csvFile << static_cast<int>(actuator.value());
        }
        csvFile << this->separator;
    }
    csvFile << "\n";
    csvFile.close();
    return core::ErrorCode::kOk;
}

  core::ErrorCode CSVDriver::WriteLine(const SensorData& data, const uint32_t& timestamp) {
    std::ofstream csvFile(this->fileName, std::ios::app);
    if (!csvFile.is_open()) {
        AppLogger::Warning("Failed to open a file: " + fileName);
        return core::ErrorCode::kError;
    }
    csvFile << timestamp << this->separator;
    std::vector<std::optional<float>> sensors;
    if (this->nozle_press) {
        sensors.push_back(data.nozle_press);
    }
    if (this->tank_press) {
        sensors.push_back(data.tank_press);
    }
    if (this->temp_1) {
        sensors.push_back(data.temp_1);
    }
    if (this->temp_2) {
        sensors.push_back(data.temp_2);
    }
    if (this->temp_3) {
        sensors.push_back(data.temp_3);
    }
    if (this->d_press) {
        sensors.push_back(data.d_press);
    }
    for (const auto& sensor : sensors) {
        if (sensor.has_value()) {
            csvFile << sensor.value();
        }
        csvFile << this->separator;
    }
    csvFile << "\n";
    csvFile.close();
    return core::ErrorCode::kOk;
  }
}  // namespace logger
}  // namespace simba
