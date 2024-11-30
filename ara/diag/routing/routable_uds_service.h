/**
 * @file routable_uds_service.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-11-30
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef ARA_DIAG_ROUTING_ROUTABLE_UDS_SERVICE_H_
#define ARA_DIAG_ROUTING_ROUTABLE_UDS_SERVICE_H_

#include <memory>
#include <string>
#include <vector>

#include "ara/com/socket/Isocket_stream.h"
#include "ara/com/socket/stream_ipc_socket.h"
#include "ara/core/instance_specifier.h"
#include "ara/core/result.h"
#include "ara/log/logger.h"

namespace ara {
namespace diag {
/// @brief Positive response of a handled UDS request
struct OperationOutput {
  /// @brief Response byte array
  std::vector<uint8_t> responseData;
};

/// @brief UDS services routing namespace
/// @note The namespace is not part of the ARA standard
namespace routing {
/// @brief An UDS service to handle a diagnostic request by offering itself to a
/// router
class RoutableUdsService {
 private:
  const uint8_t mSid;
  bool mOffered;
  const ara::core::InstanceSpecifier &mSpecifier;
  const ara::log::Logger &diag_logger_;
  std::string job_id_{""};
  const std::unique_ptr<ara::com::soc::ISocketStream> sock_;

 protected:
  /// @brief SID increment for a positive response
  static const uint8_t cPositiveResponseSidIncrement{0x40};
  /// @brief Unsupported sub-function NRC
  const uint8_t cSubFunctionNotSupported{0x12};
  /// @brief Incorrect message length NRC
  const uint8_t cIncorrectMessageLength{0x13};
  /// @brief Incorrect request sequence NRC
  const uint8_t cRequestSequenceError{0x24};

  /// @brief Constructor
  /// @param specifier Owner instance specifier
  /// @param sid UDS service ID
  RoutableUdsService(
      const ara::core::InstanceSpecifier &specifier, uint8_t sid,
      std::unique_ptr<ara::com::soc::ISocketStream> sock =
          std::make_unique<ara::com::soc::StreamIpcSocket>()) noexcept;

  /// @brief Generate a negative response with a specific NRC
  /// @param[out] response Generated negative response
  /// @param nrc Given Negative Response Code (NRC)
  void GenerateNegativeResponse(OperationOutput &response,  // NOLINT
                                uint8_t nrc) const;
  std::vector<uint8_t> RXLoop(const std::vector<uint8_t> &data);

 public:
  /// @brief Negative response code service ID
  static const uint8_t cNegativeResponseCodeSid{0x7f};

  /// @brief Get SID
  /// @returns Service identification
  uint8_t GetSid() const noexcept;

  /// @brief Offer handling DM requests
  /// @returns Error result if the service has been already offered
  ara::core::Result<void> Offer();

  /// @brief Get offering status of the service
  /// @returns True if the service has been offered, otherwise false
  bool IsOffered() const noexcept;

  /// @brief Handle an UDS request message
  /// @param requestData Request message byte array
  /// @param metaInfo Request message metainfo
  /// @param cancellationHandler Callback to be invoked when the current
  /// conversation is cancelled
  /// @returns Response byte array
  virtual ara::core::Result<OperationOutput> HandleMessage(
      const std::vector<uint8_t> &requestData) = 0;

  /// @brief Stop offering request handling
  void StopOffer() noexcept;

  virtual ~RoutableUdsService() noexcept = default;
};
}  // namespace routing
}  // namespace diag
}  // namespace ara

#endif  // ARA_DIAG_ROUTING_ROUTABLE_UDS_SERVICE_H_
