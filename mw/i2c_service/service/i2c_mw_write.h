/**
 * @file i2c_mw_write.h
 * @author Wiktor Laska
 * @brief 
 * @version 0.1
 * @date 2025-12-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef MW_I2C_READ_READ_I2C_READ_H_
#define MW_I2C_READ_READ_I2C_READ_H_
#include <string>
#include <vector>
#include <memory>
#include <sstream>
#include <unordered_map>
#include <bitset>
#include "ara/log/log.h"
#include "core/i2c/i2c_driver.hpp"
#include "ara/diag/generic_data_identifier.h"
#include "ara/diag/uds_error_domain.h"
#include "mw/i2c_service/service/i2c_service.h"
namespace srp {
namespace mw {


class I2CMWWRITE : public ara::diag::GenericDiD {
 private:
  std::shared_ptr<core::i2c::II2CDriver> i2c_;
  /**
   * @brief Callback for 0x22 UDS job (Read data by id) *optional
   *
   * @return DiagResponse
   */
  ara::core::Result<ara::diag::OperationOutput> Read() noexcept override {
    return ara::diag::MakeErrorCode(
      ara::diag::UdsDiagErrc::kRequestOutOfRange);
  }


   ara::core::Result<void> Write(
      const std::vector<uint8_t> &payload) noexcept override {
    if (payload.size()%2 != 1 || payload.size() < 3) {
      ara::log::LogWarn() << "Invalid payload size: " << payload.size();
      return ara::diag::MakeErrorCode(
        ara::diag::UdsDiagErrc::kSubFunctionNotSupported);
    }
    ara::log::LogInfo() << "Receive diag write req for address:" << payload[0] << ", with size: "
        << (uint16_t)payload.size();
    if (this->i2c_->Ioctl(payload[0]) != core::ErrorCode::kOk) {
      ara::log::LogInfo() << "Wrong Input for payload i2c write";
      return {};
    }
    std::vector<uint8_t> vec(payload.begin() + 1, payload.end());
    if (this->i2c_->Write(vec) == core::ErrorCode::kOk) {
      ara::log::LogInfo() << "Wrong Input for i2c Write";
      return ara::diag::MakeErrorCode(
        ara::diag::UdsDiagErrc::kSubFunctionNotSupported);
    }
    return {};
}

 public:
  I2CMWWRITE(const ara::core::InstanceSpecifier &specifier, std::shared_ptr<
        core::i2c::II2CDriver> i2c_d)
                        : ara::diag::GenericDiD{specifier}, i2c_(i2c_d) {
      }
};

}  // namespace mw
}  // namespace srp

#endif  
