/**
 * @file example_did.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-11-30
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef EXAMPLE_ADAPTIVE_EXAMPLEAPP_CODE_APPLICATION_EXAMPLE_DID_H_
#define EXAMPLE_ADAPTIVE_EXAMPLEAPP_CODE_APPLICATION_EXAMPLE_DID_H_

#include <vector>

#include "ara/diag/generic_data_identifier.h"
#include "ara/log/log.h"

namespace simba {
namespace example {
class ExampleDiD : public ara::diag::GenericDiD {
 private:
  ara::core::Result<ara::diag::OperationOutput> Read() noexcept override {
    ara::log::LogInfo() << "Read dziala";
    return ara::diag::OperationOutput{{1, 2, 3, 4, 5, 6, 7, 8, 9, 10}};
  }
  ara::core::Result<void> Write(
      const std::vector<uint8_t> &) noexcept override {
    ara::log::LogInfo() << "Write dziala";
    return {};
  }

 public:
  explicit ExampleDiD(const ara::core::InstanceSpecifier &specifier)
      : ara::diag::GenericDiD{specifier} {}
  ~ExampleDiD() = default;
};
}  // namespace example
}  // namespace simba

#endif  // EXAMPLE_ADAPTIVE_EXAMPLEAPP_CODE_APPLICATION_EXAMPLE_DID_H_
