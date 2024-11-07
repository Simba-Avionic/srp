/**
 * @file sim_i2cdriver.cpp
 * @author Mateusz krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-07-23
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <cstdint>
#include "core/i2c/i2cdriver.hpp"
#include "core/logger/Logger.h"
#include "communication-core/sockets/tcp_socket.h"
#include "communication-core/sockets/socket_config.h"
#include "core/json/json_parser.h"

namespace simba {
namespace core {
namespace i2c {

namespace {
    static const std::string IP = "192.168.10.1";
    constexpr auto  PORT = 2055;
}

core::ErrorCode I2CDriver::Init() {
  AppLogger::Info("I2C: init i2c driver");
    com::soc::StreamTCPSocket sock;
    nlohmann::json jsonObj;
    jsonObj["type"] = "i2c";
    jsonObj["action"] = "init";
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

core::ErrorCode I2CDriver::Ioctl(const uint8_t address, const uint16_t type) {
  AppLogger::Info("I2C: IOCTL for id: "+ std::to_string(address));
    com::soc::StreamTCPSocket sock;
    nlohmann::json jsonObj;
    jsonObj["type"] = "i2c";
    jsonObj["action"] = "ioctl";
    jsonObj["address"] = address;
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

core::ErrorCode I2CDriver::Write(const std::vector<uint8_t>& RegData) {
  std::string log;
  std::copy(RegData.begin(), RegData.end(), log.begin());
  AppLogger::Info("I2C: Write data:" + log);
    com::soc::StreamTCPSocket sock;
    nlohmann::json jsonObj;
    jsonObj["type"] = "i2c";
    jsonObj["action"] = "Write";
    jsonObj["regData"] = RegData;
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
core::ErrorCode I2CDriver::Write(const uint8_t& data) {
  AppLogger::Info("I2C: Write data:" + std::to_string(data));
    com::soc::StreamTCPSocket sock;
    nlohmann::json jsonObj;
    jsonObj["type"] = "i2c";
    jsonObj["action"] = "Write";
    jsonObj["data"] = data;
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
std::optional<std::vector<uint8_t>> I2CDriver::Read(const uint8_t size) {
  AppLogger::Info("I2C: Read data req");
    com::soc::StreamTCPSocket sock;
    nlohmann::json jsonObj;
    jsonObj["type"] = "i2c";
    jsonObj["action"] = "read";
    jsonObj["size"] = size;
    std::string jsonString = jsonObj.dump();
    std::vector<uint8_t> byteVec(jsonString.begin(), jsonString.end());
    auto res = sock.Transmit(IP, PORT, byteVec);
    if (res.has_value()) {
        if (res.value().size() > 0) {
        return res.value();
        }
    }
    return std::nullopt;
}
core::ErrorCode I2CDriver::PageWrite(std::vector<uint8_t> data) {
  std::string log;
  std::copy(data.begin(), data.end(), log.begin());
  AppLogger::Info("I2C: Write data:" + log);
    com::soc::StreamTCPSocket sock;
    nlohmann::json jsonObj;
    jsonObj["type"] = "i2c";
    jsonObj["action"] = "PageWrite";
    jsonObj["data"] = data;
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
std::optional<std::vector<uint8_t>> I2CDriver::ReadWrite(const uint8_t& reg, const uint8_t size) {
  AppLogger::Info("I2C: ReadWrite: " + std::to_string(reg) + "  to read size: " + std::to_string(size));
    com::soc::StreamTCPSocket sock;
    nlohmann::json jsonObj;
    jsonObj["type"] = "i2c";
    jsonObj["action"] = "ReadWrite";
    jsonObj["reg"] = reg;
    jsonObj["size"] = size;
    std::string jsonString = jsonObj.dump();
    std::vector<uint8_t> byteVec(jsonString.begin(), jsonString.end());
    auto res = sock.Transmit(IP, PORT, byteVec);
    if (res.has_value()) {
        if (res.value().size() > 0) {
          return res.value();
        }
    }
    return std::nullopt;
}
}  // namespace i2c
}  // namespace core
}  // namespace simba
