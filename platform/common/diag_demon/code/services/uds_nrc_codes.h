/**
 * @file uds_nrc_codes.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-04-28
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef PLATFORM_COMMON_DIAG_DEMON_CODE_SERVICES_UDS_NRC_CODES_H_
#define PLATFORM_COMMON_DIAG_DEMON_CODE_SERVICES_UDS_NRC_CODES_H_
#include <cstdint>
namespace srp {
namespace platform {
namespace diag_demon {
namespace uds {
enum UdsNrcCodes : uint8_t {
  kOk = 0x00,
  kGeneralReject = 0x10,
  kServiceNotSupported = 0x11,
  kSubFunctionNotSupported = 0x12,
  kInvalidMessageLengthFormat = 0x13,
  kResponseTooLong = 0x14,
  kBusyRepeatRequest = 0x21,
  kConditionsNotCorrect = 0x22,
  kRequestSequenceError = 0x24,
  kNoResponseFromSubnetComponent = 0x25,
  kFailurePreventsExecutionOfRequestedAction = 0x26,
  kRequestOutOfRange = 0x31,
  kSecurityAccessDenied = 0x33,
  kInvalidKey = 0x35,
  kExceededNumberOfAttempts = 0x36,
  kRequiredTimeDelayHasNotExpired = 0x37,
  kUploadDownloadNotAccepted = 0x70,
  kTransferDataSuspended = 0x71,
  kProgrammingFailure = 0x72,
  kWrongBlockSequenceCounter = 0x73,
  kRequestReceivedResponsePending = 0x78,
  kSubFunctionNotSupportedInActiveSession = 0x7E,
  kServiceNotSupportedInActiveSession = 0x7F
};
}  // namespace uds
}  // namespace diag_demon
}  // namespace platform
}  // namespace srp
#endif  // PLATFORM_COMMON_DIAG_DEMON_CODE_SERVICES_UDS_NRC_CODES_H_
