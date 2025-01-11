/**
 * @file example_did.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-09-01
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef PLATFORM_COMMON_DIAG_DEMON_CODE_SERVICES_EXAMPLE_DID_H_
#define PLATFORM_COMMON_DIAG_DEMON_CODE_SERVICES_EXAMPLE_DID_H_
#include <string>
#include <vector>

#include "ara/diag/generic_data_identifier.h"
#include "ara/diag/uds_error_domain.h"
#include "ara/log/log.h"

namespace srp {
namespace platform {
namespace diag_demon {

class ExampleDiD : public ara::diag::GenericDiD {
 private:
  ara::core::Result<ara::diag::OperationOutput> Read() noexcept override {
    return ara::diag::OperationOutput{{0x01, 0x02, 0x03, 0x04}};
  }
  ara::core::Result<void> Write(const std::vector<uint8_t>&) noexcept override {
    return ara::diag::MakeErrorCode(
        ara::diag::UdsDiagErrc::kSubFunctionNotSupported);
  }

 public:
  explicit ExampleDiD(const ara::core::InstanceSpecifier& instance)
      : ara::diag::GenericDiD(instance) {}
};
}  // namespace diag_demon
}  // namespace platform
}  // namespace srp

#endif  // PLATFORM_COMMON_DIAG_DEMON_CODE_SERVICES_EXAMPLE_DID_H_
