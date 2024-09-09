/**
 * @file log_stream.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-09-01
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef ARA_LOG_LOG_STREAM_H_
#define ARA_LOG_LOG_STREAM_H_

#include <functional>
#include <string>
#include <utility>
#include <vector>

#include "ara/core/error_code.h"
#include "ara/core/instance_specifier.h"
#include "ara/log/common.h"

namespace ara {
namespace log {
class Logger;
/// @brief A stream pipeline to combine log entities
class LogStream final {
 private:
  std::string mLogs;
  std::string app_id_{};
  std::string ctx_{};
  const LogLevel level_;
  std::function<void(const std::string &, LogLevel)> callback_;

  void concat(std::string &&log);

 public:
  void SetMetaData(const std::string &app_id, const std::string &ctx) {
    app_id_ = app_id;
    ctx_ = ctx;
  }
  LogLevel GetLogLevel() const { return level_; }
  std::string GetAppId() const { return app_id_; }
  std::string GetCtx() const { return ctx_; }
  LogStream(LogLevel level,
            std::function<void(const std::string &, LogLevel)> callback)
      : level_{level}, callback_{callback} {}
  LogStream() : level_{LogLevel::kOff} {}
  LogStream(LogLevel level) : level_{level} {}
  ~LogStream() {
    if (callback_) {
      callback_(mLogs, level_);
    }
  }
  //   LogStream(const LogStream &) = delete;
  /// @brief LogStream insertion operator
  /// @param value Another logstream
  /// @returns Reference to the current log stream
  LogStream &operator<<(const LogStream &value);

  /// @brief Boolean insertion operator
  /// @param value A boolean value
  /// @returns Reference to the current log stream
  LogStream &operator<<(bool value);

  /// @brief Byte insertion operator
  /// @param value A byte value
  /// @returns Reference to the current log stream
  LogStream &operator<<(uint8_t value);

  /// @brief Unsigned integer insertion operator
  /// @param value An unsigned integer value
  /// @returns Reference to the current log stream
  LogStream &operator<<(uint16_t value);
  /// @brief Unsigned integer insertion operator
  /// @param value An unsigned integer value
  /// @returns Reference to the current log stream
  LogStream &operator<<(uint32_t value);
  /// @brief Float insertion operator
  /// @param value A float value
  /// @returns Reference to the current log stream
  LogStream &operator<<(float value);

  /// @brief String insertion operator
  /// @param value A string
  /// @returns Reference to the current log stream
  LogStream &operator<<(const std::string &value);

  /// @brief C-syle string insertion operator
  /// @param value Character array
  /// @returns Reference to the current log stream
  LogStream &operator<<(const char *value);

  /// @brief LogLeve insertion operator
  /// @param value Log severity level
  /// @returns Reference to the current log stream
  LogStream &operator<<(LogLevel value);

  /// @brief ErrorCode insertion operator
  /// @param value An error code object
  /// @returns Reference to the current log stream
  LogStream &operator<<(const ara::core::ErrorCode &value);

  /// @brief InstanceSpecifier insertion operator
  /// @param value An instance specifier object
  /// @returns Reference to the current log stream
  LogStream &operator<<(const ara::core::InstanceSpecifier &value) noexcept;

  /// @brief Data array insertion operator
  /// @param value Data byte vector
  /// @returns Reference to the current log stream
  LogStream &operator<<(std::vector<std::uint8_t> value);

  /// @brief Log stream at a certian file and a certian line within the file
  /// @param file File name
  /// @param line Line number
  /// @returns Reference to the current log stream
  LogStream &WithLocation(std::string file, int line);

  /// @brief Convert the current log stream to a standard string
  /// @returns Serialized log stream string
  std::string ToString() const noexcept;
};
}  // namespace log
}  // namespace ara

#endif  // ARA_LOG_LOG_STREAM_H_
