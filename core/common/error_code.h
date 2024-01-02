/**
 * @file error_code.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief This file define internal error status
 * @version 0.1
 * @date 2023-10-21
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef CORE_COMMON_ERROR_CODE_H_
#define CORE_COMMON_ERROR_CODE_H_

#include <stdexcept>
#include <string>
namespace simba {
namespace core {
enum ErrorCode {
  kOk = 0,
  kNotDefine,
  kError,
  kConnectionError,
  kInitializeError,
  kBadVariableSize,
};

}  // namespace core
}  // namespace simba

namespace std {
namespace __cxx11 {

inline string to_string(const simba::core::ErrorCode& code) {
  switch (code) {
    case simba::core::ErrorCode::kOk:
      return "kOK";
      break;
    case simba::core::ErrorCode::kNotDefine:
      return "kNotDefine";
      break;
    case simba::core::ErrorCode::kError:
      return "kError";
      break;
    case simba::core::ErrorCode::kConnectionError:
      return "kConnectionError";
      break;
    case simba::core::ErrorCode::kInitializeError:
      return "kInitializeError";
      break;
    case simba::core::ErrorCode::kBadVariableSize:
      return "kBadVariableSize";
    default:
      throw std::invalid_argument("Not support error code");
      break;
  }
}
}  // namespace __cxx11
}  // namespace std
#endif  // CORE_COMMON_ERROR_CODE_H_
