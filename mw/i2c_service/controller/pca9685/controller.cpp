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
#include <utility>
#include <thread>  // NOLINT
#include <future>  // NOLINT
#include <chrono> // NOLINT

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
namespace {
    constexpr uint8_t MOSFET_DEFAULT_ON_TIME = 250;
    constexpr uint8_t SERVO_DELAY_DEFAULT_TIME = 50;
    constexpr uint8_t LOSENING_DEFAULT_DELAY = 50;
}

PCA9685::PCA9685() {
}
core::ErrorCode PCA9685::Init(const std::unordered_map<std::string, std::string>& parms,
 std::unique_ptr<II2CController> i2c, std::unique_ptr<gpio::IGPIOController> gpio) {
  if (this->setI2C(std::move(i2c)) != core::ErrorCode::kOk || this->setGPIO(std::move(gpio)) != core::ErrorCode::kOk) {
    AppLogger::Warning("Failed pointer assignment");
    return core::ErrorCode::kInitializeError;
  }
  this->i2c_->Init(std::make_unique<com::soc::StreamIpcSocket>());
  this->app_name = parms.at("app_name");
  std::string file_path = "/opt/"+this->app_name+"/etc/config.json";
  std::ifstream file(file_path);
  if (!file.is_open()) {
      AppLogger::Warning("Cant find file on path "+file_path);
      return core::ErrorCode::kInitializeError;;
  }
  nlohmann::json data = nlohmann::json::parse(file);
  auto db = this->ReadConfig(data);
  if (!db.has_value()) {
    AppLogger::Error("Servo config does not exist");
    return core::ErrorCode::kInitializeError;;
  }
  this->db_ = db.value();
  for (auto chan : this->db_) {
      this->SetServo(chan.second.channel, chan.second.off_pos);
  }
  return core::ErrorCode::kOk;
}
core::ErrorCode PCA9685::setI2C(std::unique_ptr<II2CController> i2c) {
  if (!i2c) {
    return core::ErrorCode::kInitializeError;
  }
  this->i2c_ = std::move(i2c);
  return core::ErrorCode::kOk;
}
core::ErrorCode PCA9685::setGPIO(std::unique_ptr<gpio::IGPIOController> gpio) {
  if (!gpio) {
    return core::ErrorCode::kInitializeError;
  }
  this->gpio_ = std::move(gpio);
  return core::ErrorCode::kOk;
}
void PCA9685::MosfetFunc(const uint8_t &mosfet_id, const uint8_t &mosfet_time) {
    this->gpio_->SetPinValue(mosfet_id, 1);
    std::this_thread::sleep_for(std::chrono::milliseconds(mosfet_time));
    this->gpio_->SetPinValue(mosfet_id, 0);
}

core::ErrorCode PCA9685::AutoSetServoPosition(const uint8_t &actuator_id, const uint8_t &state) {
      auto it = this->db_.find(actuator_id);
    if (it == this->db_.end()) {
        AppLogger::Warning("Not find service");
        return core::ErrorCode::kNotDefine;
    }
    if (it->second.position != state) {
        it->second.position = state;
        if (it->second.need_mosfet) {
            std::ignore = std::async(std::launch::async, &PCA9685::MosfetFunc,
                                                        this, it->second.mosfet_id, it->second.mosfet_time);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(it->second.servo_delay));
        this->SetServo(it->second.channel, it->second.position == 1 ? it->second.on_pos : it->second.off_pos);
        if (it->second.need_loosening) {
            std::this_thread::sleep_for(std::chrono::milliseconds(LOSENING_DEFAULT_DELAY));
            this->SetServo(it->second.channel,
                        it->second.position == 1 ? it->second.on_loosening : it->second.off_loosening);
        }
        return core::ErrorCode::kOk;
    }
    return core::ErrorCode::kError;
}

std::vector<uint8_t> PCA9685::GenerateData(const uint8_t &channel, const uint16_t &pos) {
    return {
    MODE1_REG, 0x01,  // setup reset mode
    PRESCALE_REG, 121,  // przeskalowanie dla 50 Hz
    static_cast<uint8_t>(LED0_ON_L+4*channel), 0x0 & 0xFF,  // ON LOW REG Val
    static_cast<uint8_t>(LED0_ON_H+4*channel), 0x0 >> 8,   // ON HIGH REG Val
    static_cast<uint8_t>(LED0_OFF_L+4*channel), static_cast<uint8_t>(pos & 0xFF),  // OFF LOW REG Val
    static_cast<uint8_t>(LED0_OFF_H+4*channel), static_cast<uint8_t>(pos >> 8)};   // OFF HIGH REG Val;
}

core::ErrorCode PCA9685::SetServo(uint8_t channel, uint16_t pos) {
  return this->i2c_->Write(PCA9685_ADDRESS, GenerateData(channel, pos));
}

std::optional<uint8_t> PCA9685::ReadServoPosition(const uint8_t &actuator_id) {
    auto servo = this->db_.find(actuator_id);
    if (servo != this->db_.end()) {
        return servo->second.position;
    }
    return {};
}

std::optional<std::unordered_map<uint8_t, Servo>> PCA9685::ReadConfig(nlohmann::json data) {
    std::unordered_map<uint8_t, Servo> db;
    if (!data.contains("servos")) {
        return {};
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
        } else if (ser.need_mosfet) {
            ser.servo_delay = SERVO_DELAY_DEFAULT_TIME;
        } else {
            ser.servo_delay = 0;
        }
        if (servo.contains("mosfet_delay")) {
            ser.mosfet_time = static_cast<uint8_t>(servo["mosfet_delay"]);
        } else if (ser.need_mosfet) {
            ser.mosfet_time = MOSFET_DEFAULT_ON_TIME;
        }
        db.insert({actuator_id, ser});
        AppLogger::Info("Register servo id:"+std::to_string(static_cast<int>(actuator_id)));
    }
    return db;
}

}  // namespace i2c
}  // namespace simba

