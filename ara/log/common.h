/**
 * @file common.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-09-01
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef ARA_LOG_COMMON_H_
#define ARA_LOG_COMMON_H_

#include <cstdint>

namespace ara {
namespace log {
/// @brief Log severity level
enum class LogLevel : std::uint8_t {
  kOff = 0x00,     ///< No logging
  kFatal = 0x01,   ///< Fatal log
  kError = 0x02,   ///< Error log
  kWarn = 0x03,    ///< Warning log
  kInfo = 0x04,    ///< Informative log
  kDebug = 0x05,   ///< Debug log
  kVerbose = 0x06  ///< Verbose log
};

/// @brief Log sink mode
enum class LogMode : std::uint8_t {
  kRemote = 0x01,  ///< Remote network logging sink
  kFile = 0x02,    ///< File logging sink for debugging
  kConsole = 0x04  ///< Console logging sink for debugging
};
inline LogMode operator|(LogMode a, LogMode b) {
  return static_cast<LogMode>(static_cast<std::uint8_t>(a) |
                              static_cast<std::uint8_t>(b));
}
inline bool operator&(LogMode a, LogMode b) {
  return (static_cast<std::uint8_t>(a) & static_cast<std::uint8_t>(b)) != 0;
}

/// @brief Logging client connection state
enum class ClientState : std::int8_t {
  kUnknown = -1,      ///< Connection state unknown
  kNotConnected = 0,  ///< Client is disconnected
  kConnected = 1      ///< Client is connected
};
}  // namespace log
}  // namespace ara

#endif  // ARA_LOG_COMMON_H_
