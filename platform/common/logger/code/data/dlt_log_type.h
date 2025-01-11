/**
 * @file dlt_log_type.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-02-07
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef PLATFORM_COMMON_LOGGER_CODE_DATA_DLT_LOG_TYPE_H_
#define PLATFORM_COMMON_LOGGER_CODE_DATA_DLT_LOG_TYPE_H_
#include <cstdint>
namespace srp {
namespace dlt {
namespace data {
enum DLTLogType : uint8_t {
  kDLTVerbose = 0x61,
  kDLTDebug = 0x51,
  kDLTInfo = 0x41,
  kDLTWarning = 0x31,
  kDLTError = 0x21,
  kDLTFatal = 0x11,
};
}  // namespace data
}  // namespace dlt
}  // namespace srp
#endif  // PLATFORM_COMMON_LOGGER_CODE_DATA_DLT_LOG_TYPE_H_
