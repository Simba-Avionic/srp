/**
 * @file generic_data_identifier.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-11-30
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef ARA_DIAG_GENERIC_DATA_IDENTIFIER_H_
#define ARA_DIAG_GENERIC_DATA_IDENTIFIER_H_

#include <vector>

#include "ara/diag/routing/routable_uds_service.h"
namespace ara {
namespace diag {
/// @brief A service to execute a specific sequence of instructions (routine)
/// and obtain the result
/// @note The class is not fully compatible with the ARA standard.
class GenericDiD : public routing::RoutableUdsService {
 private:
  static const uint8_t cSid{0x22};

 public:
  /// @brief Constructor
  /// @param specifier Instance specifier that owns the service
  /// @param reentrancyType Service reentrancy type in a multi-threading contex
  explicit GenericDiD(const core::InstanceSpecifier &specifier);

  ara::core::Result<OperationOutput> HandleMessage(
      const std::vector<uint8_t> &requestData) override;

  virtual ara::core::Result<OperationOutput> Read() noexcept = 0;
  virtual ara::core::Result<void> Write(
      const std::vector<uint8_t> &) noexcept = 0;
};
}  // namespace diag
}  // namespace ara

#endif  // ARA_DIAG_GENERIC_DATA_IDENTIFIER_H_
