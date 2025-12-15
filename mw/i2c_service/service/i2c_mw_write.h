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
  ara::log::LogInfo() << "Receive diag write req with payload size: "
      << (uint16_t)payload.size() << ",p[0] = " << payload[0] << ",p[1] = " << payload[1];
  if (payload.size()%2 != 1) {
    ara::log::LogWarn() << "Invalid payload size ";
    return ara::diag::MakeErrorCode(
      ara::diag::UdsDiagErrc::kInvalidMessageLengthFormat);
  }
  for(size_t i=1;i<payload.size();i+=2){
    this->i2c_->Write(payload[i]);
  }
  ara::log::LogInfo() << "OK";
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
