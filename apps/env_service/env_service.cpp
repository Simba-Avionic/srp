/**
 * @file env_service.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-04-04
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <memory>
#include <fstream>
#include <algorithm>
#include <filesystem>
#include "apps/env_service/env_service.hpp"
#include "mw/temp/temp_reading_msg/temp_reading_msg_factory.h"

namespace simba {
namespace envService {

namespace {
    static constexpr char const* path = "/opt/temp_service/etc/config.json";
    static constexpr char const* sensors_path = "/sys/bus/w1/devices/"
}

std::vector<uint8_t> stringToBytes(const std::string &data) {
  std::vector<uint8_t> bytes(data.size());
  std::transform(data.begin(), data.end(), bytes.begin(),
            [](char c) { return static_cast<uint8_t>(c);});
}
std::string bytesToString(const std::vector<uint8_t>& bytes) {
    std::string str(bytes.size(), '\0');
    std::transform(bytes.begin(), bytes.end(), str.begin(),
                   [](uint8_t byte) { return static_cast<char>(byte); });
    return str;
}

std::optional<uint16_t> ReadSpecificTemp(const std::string & sensor) {
    std::fstream file(sensors_path+sensor+"/temperature");
    if (!file.is_open()) {
        return {};
    }
    std::string val;
    file >> val;
    file.close();
    try {
        int tempVal = std::stoi(val);
        return tempVal/100;
    } catch (const std::exception& e) {
        return {};
    }
}

std::vector<uint8_t> GetAllTempSensors() {
    std::vector<uint8_t> res;
    for (const auto& entry : std::filesystem::directory_iterator()) {
        if (std::filesystem::is_directory(entry) && entry.path().filename().string() != "w1_bus_master1") {
          auto sensor = stringToBytes(entry.path().filename().string());
          res.insert(res.end(), sensor.begin(), sensor.end());
        }
    }
    return res;
}

std::vector<uint8_t> convertPayload(const double &value) {
    std::vector<uint8_t> bytes;
    bytes.resize(sizeof(int16_t));
    const int16_t floatValue = static_cast<int16_t>(value * 10);
    std::memcpy(bytes.data(), &floatValue, sizeof(int16_t));
    return bytes;
}
std::optional<std::string> ReadOneWireID(uint8_t actuator_id) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return {};
    }
    nlohmann::json jsonData;
    file >> jsonData;
    file.close();
    for (auto sensor : jsonData["sensors-temp"].items()) {
        if (sensor.value() == actuator_id) {
            return sensor.key();
        }
    }
    return {};
}
core::ErrorCode ChangeSensorID(uint8_t actuator_id, std::string physical_id) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return core::ErrorCode::kError;
    }
    nlohmann::json data;
    file >> data;
    file.close();
    for (auto it = data["sensors-temp"].begin(); it != data["sensors-temp"].end(); ++it) {
        if (it.value() == actuator_id) {
            it = data["sensors-temp"].erase(it);
            break;
        }
    }
    data["sensors-temp"][physical_id] = actuator_id;
    std::ofstream out(path);
    if (!out.is_open()) {
        return core::ErrorCode::kError;
    }
    out << data << std::endl;
    out.close();
    return core::ErrorCode::kOk;
}

core::ErrorCode EnvService::Run(std::stop_token token) {
    this->SleepMainThread();
    return core::ErrorCode::kOk;
}

core::ErrorCode EnvService::InitDiag() {
/**
 * @brief READ
 * 
 */
  this->diag_controller.AddMethod(1,
        [this](const std::vector<uint8_t>& payload) -> std::optional<std::vector<uint8_t>> {
    if (auto temp = this->temp_hist.find(0) != this->temp_hist.end()) {
      return std::vector<uint8_t>{
        static_cast<uint8_t>((temp >> 8) & 0xFF),
        static_cast<uint8_t>(temp & 0xFF)};
    }
    return {};
}, diag::DiagMethodType::READ);
this->diag_controller.AddMethod(2,
        [this](const std::vector<uint8_t>& payload) -> std::optional<std::vector<uint8_t>> {
    if (auto temp = this->temp_hist.find(1) != this->temp_hist.end()) {
      return std::vector<uint8_t>{
        static_cast<uint8_t>((temp >> 8) & 0xFF),
        static_cast<uint8_t>(temp & 0xFF)};
    }
    return {};
}, diag::DiagMethodType::READ);
  this->diag_controller.AddMethod(3,
        [this](const std::vector<uint8_t>& payload) -> std::optional<std::vector<uint8_t>> {
    if (auto temp = this->temp_hist.find(2) != this->temp_hist.end()) {
      return std::vector<uint8_t>{
        static_cast<uint8_t>((temp >> 8) & 0xFF),
        static_cast<uint8_t>(temp & 0xFF)};
    }
    return {};
}, diag::DiagMethodType::READ);
  this->diag_controller.AddMethod(7,
        [this](const std::vector<uint8_t>& payload) -> std::optional<std::vector<uint8_t>> {
    auto res = ReadOneWireID(0);
    if (!res.has_value()) {
        return {};
    }
    return stringToBytes(res.value());
}, diag::DiagMethodType::READ);
this->diag_controller.AddMethod(8,
        [this](const std::vector<uint8_t>& payload) -> std::optional<std::vector<uint8_t>> {
    auto res = ReadOneWireID(1);
    if (!res.has_value()) {
        return {};
    }
    return stringToBytes(res.value());
}, diag::DiagMethodType::READ);
this->diag_controller.AddMethod(9,
        [this](const std::vector<uint8_t>& payload) -> std::optional<std::vector<uint8_t>> {
    auto res = ReadOneWireID(2);
    if (!res.has_value()) {
        return {};
    }
    return stringToBytes(res.value());
}, diag::DiagMethodType::READ);
this->diag_controller.AddMethod(16,
        [this](const std::vector<uint8_t>& payload) -> std::optional<std::vector<uint8_t>> {
    return GetAllTempSensors();
}, diag::DiagMethodType::READ);
/**
 * @brief WRITE
 */
this->diag_controller.AddMethod(7,
       [this](const std::vector<uint8_t>& payload) -> std::optional<std::vector<uint8_t>> {
    auto res = ChangeSensorID(0, bytesToString(payload));
    if (res != core::ErrorCode::kOk) {
        return {};
    }
    return std::vector<uint8_t>{};
}, diag::DiagMethodType::WRITE);
this->diag_controller.AddMethod(8,
       [this](const std::vector<uint8_t>& payload) -> std::optional<std::vector<uint8_t>> {
    auto res = ChangeSensorID(1, bytesToString(payload));
    if (res != core::ErrorCode::kOk) {
        return {};
    }
    return std::vector<uint8_t>{};
}, diag::DiagMethodType::WRITE);
this->diag_controller.AddMethod(9,
       [this](const std::vector<uint8_t>& payload) -> std::optional<std::vector<uint8_t>> {
    auto res = ChangeSensorID(2, bytesToString(payload));
    if (res != core::ErrorCode::kOk) {
        return {};
    }
    return std::vector<uint8_t>{};
}, diag::DiagMethodType::WRITE);
this->diag_controller.AddMethod(1,
       [this](const std::vector<uint8_t>& payload) -> std::optional<std::vector<uint8_t>> {
    const std::string sensor_name = bytesToString(payload);
    auto res = ReadSpecificTemp(sensor_name);
    if (!res.has_value()) {
        return {};
    }
    return std::vector<uint8_t>{
        static_cast<uint8_t>((res.value() >> 8) & 0xFF),
        static_cast<uint8_t>(res.value() & 0xFF)};
}, diag::DiagMethodType::JOB);

}


core::ErrorCode EnvService::Initialize(
      const std::unordered_map<std::string, std::string>& parms) {
    this->InitDiag();
    core::ErrorCode res;
    this->temp1_event = std::make_shared<com::someip::EventSkeleton>("EnvApp/newTempEvent_1");
    this->temp2_event = std::make_shared<com::someip::EventSkeleton>("EnvApp/newTempEvent_2");
    this->temp3_event = std::make_shared<com::someip::EventSkeleton>("EnvApp/newTempEvent_3");
    com->Add(temp1_event);
    com->Add(temp2_event);
    com->Add(temp3_event);
    this->dtc_temp_error = std::make_shared<diag::dtc::DTCObject>(0x20);
    this->dtc_temp_connection_error_0xB0 = std::make_shared<diag::dtc::DTCObject>(0xB0);
    diag_controller.RegisterDTC(dtc_temp_connection_error_0xB0);
    diag_controller.RegisterDTC(dtc_temp_error);
    uint8_t i = 0;
    do {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        res = this->temp_.Init(514, std::bind(&EnvService::TempRxCallback,
            this, std::placeholders::_1, std::placeholders::_2,
                                        std::placeholders::_3));
    } while (res != core::ErrorCode::kOk && i < 6);
    if (res != core::ErrorCode::kOk) {
        this->dtc_temp_connection_error_0xB0->Failed();
        return res;
    }
    return core::ErrorCode::kOk;
}
bool EnvService::CheckTempError(const double &value) {
    if (value > 120 || value <-30) {
        dtc_temp_error->Failed();
        return false;
    } else {
        dtc_temp_error->Pass();
        return true;
    }
}

void EnvService::TempRxCallback(const std::string& ip, const std::uint16_t& port,
                                const std::vector<std::uint8_t> data) {
    mw::temp::TempReadingMsgFactory factory_;
    auto hdrs = factory_.GetPayload(data);
    for (auto &hdr : hdrs) {
        this->CheckTempError(hdr.second);
        AppLogger::Info("Receive temp id: "+std::to_string(hdr.first)+",temp:"+std::to_string(hdr.second));
        switch (hdr.first) {
            case 0:
            this->temp_hist[0] = static_cast<int16_t>(10 * hdr.second);
            this->temp1_event->SetValue(convertPayload(hdr.second));
            break;
            case 1:
            this->temp_hist[1] = static_cast<int16_t>(10 * hdr.second);
            this->temp2_event->SetValue(convertPayload(hdr.second));
            break;
            case 2:
             this->temp_hist[2] = static_cast<int16_t>(10 * hdr.second);
            this->temp3_event->SetValue(convertPayload(hdr.second));
            break;
            default:
            AppLogger::Warning("ID spoza zakresu:"+std::to_string(hdr.first));
            break;
        }
    }
}


}  // namespace envService
}  // namespace simba
