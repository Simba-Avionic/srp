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

#ifndef MW_LOGGER_CODE_DATA_DLT_LOG_TYPE_H_
#define MW_LOGGER_CODE_DATA_DLT_LOG_TYPE_H_
#include <cstdint>
namespace simba {
namespace dlt {
namespace data {
  enum DLTLogType : uint8_t {
    kDLTDebug = 0x51,
    kDLTInfo = 0x41,
    kDLTWarning = 0x31,
    kDLTError = 0x21,
  };
}  // namespace data
}  // namespace dlt
}  // namespace simba
#endif  // MW_LOGGER_CODE_DATA_DLT_LOG_TYPE_H_
