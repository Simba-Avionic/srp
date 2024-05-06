/**
 * @file controller.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-04-09
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <vector>
#include <string>
#include <thread>  // NOLINT

#include "mw/i2c_service/controller/pca9685/controller.hpp"
#include "core/json/json_parser.h"
#include "core/common/error_code.h"
#include "core/logger/Logger.h"
namespace simba {
namespace i2c {

namespace {
    constexpr uint8_t PCA9685_ADDRESS = 0x70;
    constexpr uint8_t MODE1_REG = 0x00;
    constexpr uint8_t PRESCALE_REG = 0xFE;
    constexpr uint8_t LED0_ON_L = 0x06;
    constexpr uint8_t LED0_ON_H = 0x07;
    constexpr uint8_t LED0_OFF_L = 0x08;
    constexpr uint8_t LED0_OFF_H = 0x09;
}

PCA9685::PCA9685() {
}
void PCA9685::Init(uint16_t service_id, const std::unordered_map<std::string, std::string>& parms) {
  this->app_name = parms.at("app_name");
  this->i2c_.Init(service_id);
  this->gpio_.Init(service_id);
  if (this->ReadConfig() != core::ErrorCode::kOk) {
    AppLogger::Error("Servo config does not exist");
  }
  for (auto chan : this->db_) {
      this->SetServo(chan.second.channel, chan.second.off_pos);
  }
}
core::ErrorCode PCA9685::AutoSetServoPosition(uint8_t actuator_id, uint8_t state) {
      auto it = this->db_.find(actuator_id);
    if (it == this->db_.end()) {
        AppLogger::Warning("Not find service");
        return core::ErrorCode::kNotDefine;
    }
    if (it->second.position != state) {
        if (it->second.need_mosfet) {
            this->gpio_.SetPinValue(it->second.mosfet_id, gpio::Value::HIGH);
            std::this_thread::sleep_for(std::chrono::milliseconds(it->second.servo_delay));
        }
        this->SetServo(it->second.channel, it->second.position == 1 ? it->second.on_pos : it->second.off_pos);
        if (it->second.need_loosening) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            this->SetServo(it->second.channel,
                        it->second.position == 1 ? it->second.on_loosening : it->second.off_loosening);
        }
        if (it->second.need_mosfet) {
            std::this_thread::sleep_for(std::chrono::milliseconds(it->second.mosfet_delay));
            this->gpio_.SetPinValue(it->second.mosfet_id, gpio::Value::LOW);
        }
        it->second.position = state;
    }
    return core::ErrorCode::kOk;
}

core::ErrorCode PCA9685::SetServo(uint8_t channel, uint16_t pos) {
  std::vector<uint8_t> data = {
    MODE1_REG, 0x01,  // setup reset mode
    PRESCALE_REG, 121,  // przeskalowanie dla 50 Hz
    static_cast<uint8_t>(LED0_ON_L+4*channel), 0x0 & 0xFF,  // ON LOW REG Val
    static_cast<uint8_t>(LED0_ON_H+4*channel), 0x0 >> 8,   // ON HIGH REG Val
    static_cast<uint8_t>(LED0_OFF_L+4*channel), static_cast<uint8_t>(pos & 0xFF),  // OFF LOW REG Val
    static_cast<uint8_t>(LED0_OFF_H+4*channel), static_cast<uint8_t>(pos >> 8)};   // OFF HIGH REG Val;
  return this->i2c_.Write(PCA9685_ADDRESS, data);
}

std::optional<uint8_t> PCA9685::ReadServoPosition(uint8_t actuator_id) {
    auto servo = this->db_.find(actuator_id);
    if (servo != this->db_.end()) {
        return servo->second.position;
    }
    return {};
}

core::ErrorCode PCA9685::ReadConfig() {
    std::ifstream file("/opt/"+this->app_name+"/etc/config.json");
    if (!file.is_open()) {
        AppLogger::Warning("Cant find file on path /opt/"+this->app_name+"/etc/config.json");
        return core::ErrorCode::kInitializeError;
    }
    nlohmann::json data = nlohmann::json::parse(file);
    if (!data.contains("servos")) {
        return core::ErrorCode::kNotDefine;
    }
    for (const auto& servo : data["servos"]) {
        if (!servo.contains("actuator_id") || !servo.contains("channel")
        || !servo.contains("on_pos") || !servo.contains("off_pos")) {
            AppLogger::Warning("Invalid servo config");
            continue;
        }
        Servo ser;
        uint8_t actuator_id = static_cast<uint8_t>(servo["actuator_id"]);
        ser.channel = static_cast<uint8_t>(servo["channel"]);
        ser.on_pos = static_cast<uint16_t>(servo["on_pos"]);
        ser.off_pos = static_cast<uint16_t>(servo["off_pos"]);

        if (servo.contains("mosfet_id")) {
            ser.need_mosfet = true;
            ser.mosfet_id = static_cast<uint8_t>(servo["mosfet_id"]);
        }
        if (servo.contains("on_losening_pos") && servo.contains("off_losening_pos")) {
            ser.need_loosening = true;
            ser.on_loosening = static_cast<uint16_t>(servo["on_losening_pos"]);
            ser.off_loosening = static_cast<uint16_t>(servo["off_losening_pos"]);
        }
        if (servo.contains("servo_delay")) {
            ser.servo_delay = static_cast<uint8_t>(servo["servo_delay"]);
        }
        if (servo.contains("mosfet_delay")) {
            ser.mosfet_delay = static_cast<uint8_t>(servo["mosfet_delay"]);
        }
        this->db_.insert({actuator_id, ser});
        AppLogger::Debug("Register servo id:"+std::to_string(static_cast<int>(actuator_id)));
    }
    return core::ErrorCode::kOk;
}

}  // namespace i2c
}  // namespace simba

