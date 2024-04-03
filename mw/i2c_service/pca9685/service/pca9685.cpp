/**
 * @file pca9685.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-03-24
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "pca9685.hpp"
#include <utility>
#include <string>

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
/**
 * @brief 
 * 
 * @param channel (0-15) 
 * @param pos (0-4096)
 * @return core::ErrorCode 
 */
core::ErrorCode PCA9685::SetServo(uint8_t channel, uint16_t pos) {
    std::unique_lock<std::mutex> lock(*this->mtx);
    this->i2c_->Ioctl(PCA9685_ADDRESS, I2C_SLAVE);
    std::vector<std::pair<uint8_t, uint8_t>> vec = {
        {MODE1_REG, 0x01},  // setup reset mode
        {PRESCALE_REG, 121},  // przeskalowanie dla 50 Hz
        {LED0_ON_L+4*channel, 0x0},  // ON LOW REG Val
        {LED0_ON_H+4*channel, 0x0},   // ON HIGH REG Val
        {LED0_OFF_L+4*channel, pos & 0xFF},  // OFF LOW REG Val
        {LED0_OFF_H+4*channel, pos >> 8}};   // OFF HIGH REG Val
    return this->i2c_->Write(vec);
}

core::ErrorCode PCA9685::Init(uint16_t app_id) {
    this->app_id = app_id;
    this->gpio_->Init(app_id);

    this->ReadConfig();
    for (auto chan : this->db_) {
        this->SetServo(chan.second.channel, chan.second.off_pos);
    }
    return core::ErrorCode::kOk;
}
core::ErrorCode PCA9685::ReadConfig() {
    std::ifstream file("/opt/i2c/etc/config.json");
    if (!file.is_open()) {
        return core::ErrorCode::kInitializeError;
    }
    nlohmann::json data = nlohmann::json::parse(file);
    if (!data.contains("servos")) {
        return core::ErrorCode::kNotDefine;
    }
    for (const auto& servo : data["servos"]) {
        Servo ser;
        uint16_t actuator_id = static_cast<uint16_t>(servo["actuator_id"]);
        ser.channel = static_cast<uint16_t>(servo["channel"]);
        ser.on_pos = static_cast<uint16_t>(servo["on_pos"]);
        ser.off_pos = static_cast<uint16_t>(servo["off_pos"]);
        ser.mosfet_id = static_cast<uint8_t>(servo["mosfet_id"]);
        this->db_.insert({actuator_id, ser});
    }

    return core::ErrorCode::kOk;
}

core::ErrorCode PCA9685::AutoSetServoPos(uint8_t actuator_id, uint8_t position) {
    auto it = this->db_.find(actuator_id);
    if (it == this->db_.end()) {
        AppLogger::Warning("Not find service");
        return core::ErrorCode::kNotDefine;
    }
    if (it->second.position != (position == 0 ? it->second.on_pos : it->second.off_pos)) {
        this->gpio_->SetPinValue(it->second.mosfet_id, gpio::Value::HIGH);
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        this->SetServo(it->second.channel, it->second.position == 1 ? it->second.on_pos : it->second.off_pos);
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        this->gpio_->SetPinValue(it->second.mosfet_id, gpio::Value::LOW);
        it->second.position = position;
    }
    return core::ErrorCode::kOk;
}

core::ErrorCode PCA9685::ManSetServoPos(uint8_t actuator_id, uint16_t position) {
    auto it = this->db_.find(actuator_id);
    if (it == this->db_.end()) {
        AppLogger::Warning("Not find service");
        return core::ErrorCode::kNotDefine;
    }
    if (it->second.position != position) {
        this->gpio_->SetPinValue(it->second.mosfet_id, gpio::Value::HIGH);
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        this->SetServo(it->second.channel, position);
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        this->gpio_->SetPinValue(it->second.mosfet_id, gpio::Value::LOW);
        it->second.position = position;
    }
    return core::ErrorCode::kOk;
}
PCA9685::PCA9685(std::mutex *mtx, std::unique_ptr<gpio::GPIOController> gpio,
        std::unique_ptr<core::I2CDriver> i2c):gpio_(std::move(gpio)), i2c_(std::move(i2c)) {
    this->mtx = mtx;
}
PCA9685::PCA9685(std::mutex *mtx) {
    this->mtx = mtx;
}

}  // namespace i2c
}  // namespace simba
