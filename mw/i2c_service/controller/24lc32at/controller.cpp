/**
 * @file controller.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 24LC32AT EEPROM Controller
 * @version 0.1
 * @date 2024-12-19
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <chrono>  // NOLINT
#include <string>
#include <thread>  // NOLINT
#include <utility>
#include <vector>

#include "mw/i2c_service/controller/24lc32at/controller.hpp"
#include "core/common/error_code.h"

namespace srp {
namespace i2c {

namespace {
    constexpr uint8_t DEFAULT_EEPROM_ADDRESS = 0x50;
    constexpr uint16_t EEPROM_SIZE_BYTES = 4096;  // 4KB = 32Kbit
    constexpr uint16_t EEPROM_MAX_ADDRESS = 0x0FFF;
    constexpr uint8_t PAGE_SIZE = 32;
    constexpr uint16_t WRITE_CYCLE_TIME_MS = 5;
}  // namespace

EEPROM24LC32AT::EEPROM24LC32AT(uint8_t address)
    : device_address_{address},
      eeprom_logger_{
          ara::log::LoggingMenager::GetInstance()->CreateLogger("24lc32at", "", ara::log::LogLevel::kDebug)} {
}

core::ErrorCode EEPROM24LC32AT::Init(std::unique_ptr<II2CController> i2c) {
  eeprom_logger_.LogInfo() << "EEPROM24LC32AT.Init: starting initialization";
  if (this->setI2C(std::move(i2c)) != core::ErrorCode::kOk) {
    eeprom_logger_.LogWarn() << "EEPROM24LC32AT.Init: failed pointer assignment";
    return core::ErrorCode::kInitializeError;
  }
  this->i2c_->Init(std::make_unique<com::soc::StreamIpcSocket>());
  if (this->InitializeEEPROM() != core::ErrorCode::kOk) {
    eeprom_logger_.LogWarn() << "EEPROM24LC32AT.Init: failed to initialize EEPROM chip";
    return core::ErrorCode::kInitializeError;
  }
  eeprom_logger_.LogInfo() << "EEPROM24LC32AT.Init: initialization completed";
  return core::ErrorCode::kOk;
}

core::ErrorCode EEPROM24LC32AT::InitializeEEPROM() {
  // Verify EEPROM is accessible by reading from address 0
  auto test_read = this->ReadByte(0x0000);
  if (!test_read.has_value()) {
    eeprom_logger_.LogWarn() << "EEPROM24LC32AT.InitializeEEPROM: failed to read from EEPROM";
    return core::ErrorCode::kInitializeError;
  }
  eeprom_logger_.LogDebug() << "EEPROM24LC32AT.InitializeEEPROM: chip verified";
  return core::ErrorCode::kOk;
}

core::ErrorCode EEPROM24LC32AT::setI2C(std::unique_ptr<II2CController> i2c) {
  if (!i2c) {
    return core::ErrorCode::kInitializeError;
  }
  this->i2c_ = std::move(i2c);
  eeprom_logger_.LogDebug() << "EEPROM24LC32AT.setI2C: controller assigned";
  return core::ErrorCode::kOk;
}

std::vector<uint8_t> EEPROM24LC32AT::GenerateAddressBytes(uint16_t address) const {
  // 24LC32AT uses 12-bit addressing, sent as 2 bytes (high byte, low byte)
  return {static_cast<uint8_t>((address >> 8) & 0x0F), static_cast<uint8_t>(address & 0xFF)};
}

core::ErrorCode EEPROM24LC32AT::WaitForWriteCycle() const {
  // EEPROM write cycle time is typically 5ms
  std::this_thread::sleep_for(std::chrono::milliseconds(WRITE_CYCLE_TIME_MS));
  return core::ErrorCode::kOk;
}

core::ErrorCode EEPROM24LC32AT::WriteByte(uint16_t address, uint8_t data) {
  if (address > EEPROM_MAX_ADDRESS) {
    eeprom_logger_.LogWarn() << "EEPROM24LC32AT.WriteByte: address out of range " <<
                                std::to_string(address);
    return core::ErrorCode::kError;
  }

  eeprom_logger_.LogDebug() << "EEPROM24LC32AT.WriteByte: address " <<
                               std::to_string(address) << " data " <<
                               std::to_string(static_cast<int>(data));

  auto address_bytes = GenerateAddressBytes(address);
  std::vector<uint8_t> write_data = {address_bytes[0], address_bytes[1], data};

  if (this->i2c_->Write(device_address_, write_data) != core::ErrorCode::kOk) {
    eeprom_logger_.LogWarn() << "EEPROM24LC32AT.WriteByte: failed to write byte";
    return core::ErrorCode::kConnectionError;
  }

  WaitForWriteCycle();

  // Verify write by reading back the data
  auto read_result = ReadByte(address);
  if (!read_result.has_value()) {
    eeprom_logger_.LogWarn() << "EEPROM24LC32AT.WriteByte: failed to verify write (read failed)";
    return core::ErrorCode::kConnectionError;
  }

  if (read_result.value() != data) {
    eeprom_logger_.LogWarn() << "EEPROM24LC32AT.WriteByte: write verification failed - "
                                "expected 0x" << std::to_string(static_cast<int>(data)) <<
                                " but read 0x" << std::to_string(static_cast<int>(read_result.value()));
    return core::ErrorCode::kError;
  }

  eeprom_logger_.LogDebug() << "EEPROM24LC32AT.WriteByte: write verified successfully";
  return core::ErrorCode::kOk;
}

core::ErrorCode EEPROM24LC32AT::WritePage(uint16_t address, const std::vector<uint8_t>& data) {
  if (address > EEPROM_MAX_ADDRESS) {
    eeprom_logger_.LogWarn() << "EEPROM24LC32AT.WritePage: address out of range " <<
                                std::to_string(address);
    return core::ErrorCode::kError;
  }

  if (data.empty()) {
    eeprom_logger_.LogWarn() << "EEPROM24LC32AT.WritePage: empty data vector";
    return core::ErrorCode::kError;
  }

  // Check if write crosses page boundary (32 bytes per page)
  uint16_t page_start = (address / PAGE_SIZE) * PAGE_SIZE;
  uint16_t page_end = page_start + PAGE_SIZE - 1;
  uint16_t write_end = address + static_cast<uint16_t>(data.size()) - 1;

  if (write_end > EEPROM_MAX_ADDRESS) {
    eeprom_logger_.LogWarn() << "EEPROM24LC32AT.WritePage: write exceeds EEPROM size";
    return core::ErrorCode::kError;
  }

  if (write_end > page_end) {
    eeprom_logger_.LogWarn() << "EEPROM24LC32AT.WritePage: write crosses page boundary";
    return core::ErrorCode::kError;
  }

  // Limit to page size
  size_t write_size = (data.size() > PAGE_SIZE) ? PAGE_SIZE : data.size();

  eeprom_logger_.LogDebug() << "EEPROM24LC32AT.WritePage: address " <<
                               std::to_string(address) << " size " <<
                               std::to_string(write_size);

  auto address_bytes = GenerateAddressBytes(address);
  std::vector<uint8_t> write_data;
  write_data.reserve(2 + write_size);
  write_data.push_back(address_bytes[0]);
  write_data.push_back(address_bytes[1]);
  write_data.insert(write_data.end(), data.begin(), data.begin() + write_size);

  if (this->i2c_->PageWrite(device_address_, write_data) != core::ErrorCode::kOk) {
    eeprom_logger_.LogWarn() << "EEPROM24LC32AT.WritePage: failed to write page";
    return core::ErrorCode::kConnectionError;
  }

  WaitForWriteCycle();

  // Verify write by reading back the data
  auto read_result = ReadSequential(address, static_cast<uint8_t>(write_size));
  if (!read_result.has_value()) {
    eeprom_logger_.LogWarn() << "EEPROM24LC32AT.WritePage: failed to verify write (read failed)";
    return core::ErrorCode::kConnectionError;
  }

  // Compare written data with read data
  bool verification_failed = false;
  if (read_result.value().size() != write_size) {
    verification_failed = true;
    eeprom_logger_.LogWarn() << "EEPROM24LC32AT.WritePage: write verification failed - "
                                "expected " << std::to_string(write_size) << " bytes but read " <<
                                std::to_string(read_result.value().size()) << " bytes";
  } else {
    for (size_t i = 0; i < write_size; ++i) {
      if (read_result.value()[i] != data[i]) {
        verification_failed = true;
        eeprom_logger_.LogWarn() << "EEPROM24LC32AT.WritePage: write verification failed - "
                                    "byte " << std::to_string(i) << " expected 0x" << std::to_string(data[i])
                                    << " but read 0x" <<
                                    std::to_string(static_cast<int>(read_result.value()[i]));
        break;
      }
    }
  }

  if (verification_failed) {
    return core::ErrorCode::kError;
  }

  eeprom_logger_.LogDebug() << "EEPROM24LC32AT.WritePage: write verified successfully";
  return core::ErrorCode::kOk;
}

std::optional<uint8_t> EEPROM24LC32AT::ReadByte(uint16_t address) {
  if (address > EEPROM_MAX_ADDRESS) {
    eeprom_logger_.LogWarn() << "EEPROM24LC32AT.ReadByte: address out of range " <<
                                 std::to_string(address);
    return std::nullopt;
  }

  eeprom_logger_.LogDebug() << "EEPROM24LC32AT.ReadByte: address " <<
                                std::to_string(address);

  auto address_bytes = GenerateAddressBytes(address);
  // For EEPROM: write 2-byte address, then read
  if (this->i2c_->Write(device_address_, address_bytes) != core::ErrorCode::kOk) {
    eeprom_logger_.LogWarn() << "EEPROM24LC32AT.ReadByte: failed to set address";
    return std::nullopt;
  }

  auto result = this->i2c_->Read(device_address_, address_bytes[1], 1);
  if (!result.has_value() || result.value().empty()) {
    eeprom_logger_.LogWarn() << "EEPROM24LC32AT.ReadByte: failed to read byte";
    return std::nullopt;
  }

  uint8_t data = result.value()[0];
  eeprom_logger_.LogDebug() << "EEPROM24LC32AT.ReadByte: address " <<
                                std::to_string(address) << " data " <<
                                std::to_string(static_cast<int>(data));
  return data;
}

std::optional<std::vector<uint8_t>> EEPROM24LC32AT::ReadSequential(uint16_t address, uint8_t size) {
  if (address > EEPROM_MAX_ADDRESS) {
    eeprom_logger_.LogWarn() << "EEPROM24LC32AT.ReadSequential: address out of range " <<
                                 std::to_string(address);
    return std::nullopt;
  }

  if (size == 0) {
    eeprom_logger_.LogWarn() << "EEPROM24LC32AT.ReadSequential: invalid size";
    return std::nullopt;
  }

  uint16_t read_end = address + static_cast<uint16_t>(size) - 1;
  if (read_end > EEPROM_MAX_ADDRESS) {
    eeprom_logger_.LogWarn() << "EEPROM24LC32AT.ReadSequential: read exceeds EEPROM size";
    return std::nullopt;
  }

  eeprom_logger_.LogDebug() << "EEPROM24LC32AT.ReadSequential: address " <<
                                std::to_string(address) << " size " <<
                                std::to_string(static_cast<int>(size));

  auto address_bytes = GenerateAddressBytes(address);
  if (this->i2c_->Write(device_address_, address_bytes) != core::ErrorCode::kOk) {
    eeprom_logger_.LogWarn() << "EEPROM24LC32AT.ReadSequential: failed to set address";
    return std::nullopt;
  }

  auto result = this->i2c_->Read(device_address_, address_bytes[1], size);
  if (!result.has_value() || result.value().empty()) {
    eeprom_logger_.LogWarn() << "EEPROM24LC32AT.ReadSequential: failed to read data";
    return std::nullopt;
  }

  eeprom_logger_.LogDebug() << "EEPROM24LC32AT.ReadSequential: read " <<
                                std::to_string(result.value().size()) << " bytes";
  return result;
}

}  // namespace i2c
}  // namespace srp

