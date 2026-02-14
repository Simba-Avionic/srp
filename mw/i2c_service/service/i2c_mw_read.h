#ifndef MW_I2C_READ_I2C_READ_H_
#define MW_I2C_READ_I2C_READ_H_

#include <vector>
#include <memory>
#include "ara/log/log.h"
#include "core/i2c/i2c_driver.hpp"
#include "ara/diag/generic_data_identifier.h"
#include "ara/diag/uds_error_domain.h"

namespace srp {
namespace mw {

class I2CService;

class I2CMWREAD : public ara::diag::GenericDiD {
 private:
  srp::mw::I2CService* i2c_service_;
  std::shared_ptr<core::i2c::II2CDriver> i2c_;
  ara::core::Result<ara::diag::OperationOutput> Read() noexcept override {
    return ara::diag::MakeErrorCode(ara::diag::UdsDiagErrc::kRequestOutOfRange);
  }


  ara::core::Result<void> Write(const std::vector<uint8_t> &payload) noexcept override;

 public:
  I2CMWREAD(const ara::core::InstanceSpecifier &specifier, I2CService* service,std::shared_ptr<
        core::i2c::II2CDriver> i2c_);
};

}  // namespace mw
}  // namespace srp

#endif