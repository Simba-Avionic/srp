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
  auto db_opt = ReadConfig(file_path);
  if (!db_opt.has_value()) {
    AppLogger::Error("cant find config in path:"+file_path);
    exit(1);
  }
  this->db_ = db_opt.value();
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

std::vector<uint8_t> PCA9685::GenerateData(const uint8_t &channel, const uint16_t &pos) const {
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

std::optional<std::unordered_map<uint8_t, Servo>> PCA9685::ReadConfig(std::string file_path) {
    std::unordered_map<uint8_t, Servo> db;
    auto parser = core::json::JsonParser::Parser(file_path);
    if (!parser.has_value()) {
        return std::nullopt;
    }
    auto arr = parser.value().GetArray("servos");
    if (!arr.has_value()) {
        return std::nullopt;
    }
    for (auto &servo : arr.value()) {
        auto parser_opt = core::json::JsonParser::Parser(servo);
        if (!parser.has_value()) {
            AppLogger::Warning("invalid data in servos config");
            continue;
        }
        auto parser = parser_opt.value();
        auto actuator_id = parser.GetNumber<uint8_t>("actuator_id");
        auto channel = parser.GetNumber<uint8_t>("channel");
        auto on_poss = parser.GetNumber<uint16_t>("on_pos");
        auto off_poss = parser.GetNumber<uint16_t>("off_pos");
        if (!(actuator_id.has_value() && channel.has_value() &&
                                on_poss.has_value() && off_poss.has_value())) {
            AppLogger::Warning("invalid config for servo");
            continue;
        }
        Servo ser;
        ser.channel = channel.value();
        ser.on_pos = on_poss.value();
        ser.off_pos = off_poss.value();
        auto mosfet_id = parser.GetNumber<uint8_t>("mosfet_id");
        if (mosfet_id.has_value()) {
            ser.need_mosfet = true;
            ser.mosfet_id = mosfet_id.value();
            ser.servo_delay = SERVO_DELAY_DEFAULT_TIME;
            ser.mosfet_time = MOSFET_DEFAULT_ON_TIME;
        }
        auto on_losening_pos = parser.GetNumber<uint16_t>("on_losening_pos");
        auto off_losening_pos = parser.GetNumber<uint16_t>("off_losening_pos");
        if (on_losening_pos.has_value() && off_losening_pos.has_value()) {
            ser.need_loosening = true;
            ser.on_loosening = on_losening_pos.value();
            ser.off_loosening = off_losening_pos.value();
        }
        auto servo_delay = parser.GetNumber<uint16_t>("servo_delay");
        if (servo_delay.has_value()) {
            ser.servo_delay = servo_delay.value();
        }
        auto mosfet_delay = parser.GetNumber<uint16_t>("mosfet_delay");
        if (mosfet_delay.has_value()) {
            ser.mosfet_time = mosfet_delay.value();
        }
        db.insert({actuator_id.value(), ser});
         AppLogger::Info("Register servo id:"+std::to_string(static_cast<int>(actuator_id.value())));
    }
    return db;
}

}  // namespace i2c
}  // namespace simba

