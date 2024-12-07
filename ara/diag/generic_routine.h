/**
 * @file generic_routine.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-11-30
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef ARA_DIAG_GENERIC_ROUTINE_H_
#define ARA_DIAG_GENERIC_ROUTINE_H_

#include <vector>

#include "ara/diag/routing/routable_uds_service.h"
namespace ara {
namespace diag {
/// @brief A service to execute a specific sequence of instructions (routine)
/// and obtain the result
/// @note The class is not fully compatible with the ARA standard.
class GenericRoutine : public routing::RoutableUdsService {
 private:
  static const uint8_t cSid{0x31};

 public:
  /// @brief Constructor
  /// @param specifier Instance specifier that owns the service
  /// @param reentrancyType Service reentrancy type in a multi-threading contex
  explicit GenericRoutine(const core::InstanceSpecifier &specifier);

  ara::core::Result<OperationOutput> HandleMessage(
      const std::vector<uint8_t> &requestData) override;

  /// @brief Request starting a routine
  /// @param requestData Start request byte array
  virtual ara::core::Result<OperationOutput> Start(
      std::vector<uint8_t> requestData) = 0;

  /// @brief Request stopping a routine
  /// @param requestData Stop request byte array
  virtual ara::core::Result<OperationOutput> Stop(
      std::vector<uint8_t> requestData) = 0;

  /// @brief Request the results of a routine
  /// @param requestData Results request byte array
  virtual ara::core::Result<OperationOutput> RequestResults(
      std::vector<uint8_t> requestData) = 0;
};
}  // namespace diag
}  // namespace ara

#endif  // ARA_DIAG_GENERIC_ROUTINE_H_
