/**
 * @file GPIO_digital_driver.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-10-31
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <chrono>
#include <fstream>
#include <iostream>
#include <cstdint>

#include "GPIO_digital_driver.h"
#include "core/logger/Logger.h"
#include "core/json/json_parser.h"
#include "core/common/error_code.h"
#include "communication-core/sockets/tcp_socket.h"
#include "communication-core/sockets/socket_config.h"

namespace simba {
namespace core {
namespace gpio {

namespace {
    static const std::string IP = "192.168.10.1";
    constexpr auto  PORT = 2055;
}


GpioDigitalDriver::GpioDigitalDriver() {}

GpioDigitalDriver::~GpioDigitalDriver() {}

core::ErrorCode  GpioDigitalDriver::unregisterPin(uint8_t pinNumber) {
    AppLogger::Info("GPIO: Unregister pin_num:"+std::to_string(pinNumber));
    com::soc::StreamTCPSocket sock;
    nlohmann::json jsonObj;
    jsonObj["type"] = "gpio";
    jsonObj["action"] = "unregister";
    jsonObj["pinNumber"] = pinNumber;
    std::string jsonString = jsonObj.dump();
    std::vector<uint8_t> byteVec(jsonString.begin(), jsonString.end());
    auto res = sock.Transmit(IP, PORT, byteVec);
    if (res.has_value()) {
        if (res.value().size() > 0) {
        return (res.value()[0] == 1) ? core::ErrorCode::kOk : core::ErrorCode::kError;
        }
    }
    return core::ErrorCode::kConnectionError;
}

core::ErrorCode GpioDigitalDriver::initializePin(uint8_t pinNumber, direction_t direction) {
    AppLogger::Info("GPIO: Initialize pin_num:"+std::to_string(pinNumber)+ ", direction"+ std::to_string(direction));
    com::soc::StreamTCPSocket sock;
    nlohmann::json jsonObj;
    jsonObj["type"] = "gpio";
    jsonObj["action"] = "init";
    jsonObj["pinNumber"] = pinNumber;
    jsonObj["direction"] = static_cast<uint8_t>(direction);
    std::string jsonString = jsonObj.dump();
    std::vector<uint8_t> byteVec(jsonString.begin(), jsonString.end());
    auto res = sock.Transmit(IP, PORT, byteVec);
    if (res.has_value()) {
        if (res.value().size() > 0) {
        return (res.value()[0] == 1) ? core::ErrorCode::kOk : core::ErrorCode::kError;
        }
    }
    return core::ErrorCode::kConnectionError;
}


std::string GpioDigitalDriver::getEndpointPath(uint8_t pinNumber, std::string endpoint) {
    return this->path+"/gpio"+std::to_string(pinNumber)+"/"+endpoint;
}

core::ErrorCode GpioDigitalDriver::setValue(uint8_t pinNumber , uint8_t value) {
    AppLogger::Info("GPIO: Set pin_num:"+std::to_string(pinNumber)+", value:" + std::to_string(value));
    com::soc::StreamTCPSocket sock;
    nlohmann::json jsonObj;
    jsonObj["type"] = "gpio";
    jsonObj["action"] = "set";
    jsonObj["pinNumber"] = pinNumber;
    jsonObj["value"] = value;
    std::string jsonString = jsonObj.dump();
    std::vector<uint8_t> byteVec(jsonString.begin(), jsonString.end());
    auto res = sock.Transmit(IP, PORT, byteVec);
    if (res.has_value()) {
        if (res.value().size() > 0) {
        return (res.value()[0] == 1) ? core::ErrorCode::kOk : core::ErrorCode::kError;
        }
    }
    return core::ErrorCode::kConnectionError;
}

core::ErrorCode GpioDigitalDriver::setDirection(uint8_t pinNumber , direction_t direction) {
    AppLogger::Info("GPIO: Set Direction pin_num:"+std::to_string(pinNumber)+", direction: " + std::to_string(direction));
    com::soc::StreamTCPSocket sock;
    nlohmann::json jsonObj;
    jsonObj["type"] = "gpio";
    jsonObj["action"] = "setDirection";
    jsonObj["pinNumber"] = pinNumber;
    jsonObj["direction"] = std::to_string(direction);
    std::string jsonString = jsonObj.dump();
    std::vector<uint8_t> byteVec(jsonString.begin(), jsonString.end());
    auto res = sock.Transmit(IP, PORT, byteVec);
    if (res.has_value()) {
        if (res.value().size() > 0) {
        return (res.value()[0] == 1) ? core::ErrorCode::kOk : core::ErrorCode::kError;
        }
    }
    return core::ErrorCode::kConnectionError;
}

uint8_t GpioDigitalDriver::getValue(uint8_t pinNumber) {
    AppLogger::Info("GPIO: Get Value of pin_num:"+std::to_string(pinNumber));
    com::soc::StreamTCPSocket sock;
    nlohmann::json jsonObj;
    jsonObj["type"] = "gpio";
    jsonObj["action"] = "get";
    jsonObj["pinNumber"] = pinNumber;
    std::string jsonString = jsonObj.dump();
    std::vector<uint8_t> byteVec(jsonString.begin(), jsonString.end());
    auto res = sock.Transmit(IP, PORT, byteVec);
    if (res.has_value()) {
        if (res.value().size() > 0) {
        return res.value()[0];
        }
    }
    return 2;
}


direction_t GpioDigitalDriver::getDirection(uint8_t pinNumber) {
    AppLogger::Info("GPIO: Get Direction pin_num:"+std::to_string(pinNumber));
    com::soc::StreamTCPSocket sock;
    nlohmann::json jsonObj;
    jsonObj["type"] = "gpio";
    jsonObj["action"] = "getDirection";
    jsonObj["pinNumber"] = pinNumber;
    std::string jsonString = jsonObj.dump();
    std::vector<uint8_t> byteVec(jsonString.begin(), jsonString.end());
    auto res = sock.Transmit(IP, PORT, byteVec);
    if (res.has_value()) {
        if (res.value().size() > 0) {
        return (res.value()[0] == direction_t::IN) ? direction_t::IN : direction_t::OUT;
        }
    }
    return direction_t::ERROR;
}

}  // namespace gpio
}  // namespace core
}  // namespace simba
