/**
 * @file logging_menager.cpp
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-09-01
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "ara/log/logging_menager.h"

#include <utility>

#include "ara/log/sink/console_log_sink.h"
#include "ara/log/sink/remote_log_sink.h"
namespace ara {
namespace log {
namespace {}  // namespace
std::unique_ptr<LoggingMenager> LoggingMenager::loger_f_{nullptr};
LoggingMenager::LoggingMenager(const std::string &appId, LogLevel logLevel)
    : appId_{appId}, mDefaultLogLevel{logLevel} {}

const Logger &LoggingMenager::CreateLogger(std::string ctxId) {
  return CreateLogger(ctxId, "", mDefaultLogLevel);
}

const Logger &LoggingMenager::CreateLogger(std::string ctxId,
                                           std::string ctxDescription) {
  return CreateLogger(ctxId, ctxDescription, mDefaultLogLevel);
}

const Logger &LoggingMenager::CreateLogger(std::string ctxId,
                                           std::string ctxDescription,
                                           LogLevel ctxDefLogLevel) {
  {
    auto iter = mLoggers.find(ctxId);
    if (iter != mLoggers.end()) {
      return iter->second;
    }
  }
  Logger _logger =
      Logger::CreateLogger(ctxId, ctxDescription, ctxDefLogLevel,
                           [this](const Logger &logger, LogLevel logLevel,
                                  const std::string &logStream) {
                             this->Log(logger, logLevel, logStream);
                           });
  std::pair<std::unordered_map<std::string, ara::log::Logger>::iterator, bool>
      iter = mLoggers.insert(std::pair(ctxId, std::move(_logger)));

  return iter.first->second;
}

void LoggingMenager::Log(const Logger &logger, LogLevel logLevel,
                         const std::string &logStream) {
  bool _isLevelEnabled = logger.IsEnabled(logLevel);
  if (_isLevelEnabled) {
    LogStream _logStreamContex{logLevel};
    _logStreamContex << logStream;
    _logStreamContex.SetMetaData(this->appId_, logger.GetContext());
    for (auto &sink_ : sink_list_) {
      sink_->Log(_logStreamContex);
    }
  }
}
void LoggingMenager::Clear() {
  if (loger_f_ != nullptr) {
    loger_f_->sink_list_.clear();
  }
}
void LoggingMenager::Create(std::string appId, LogMode logMode,
                            LogLevel logLevel, std::string appDescription) {
  if (loger_f_ != nullptr) {
    const auto &ara_logger = loger_f_->CreateLogger("ara-", "");
    ara_logger.LogError()
        << "An attempt to double initialize LoggingMenager was detected";
    return;
  }
  //   if (logMode == LogMode::kFile) {
  //     throw std::invalid_argument(
  //         "File logging mode is not feasible within this constructor
  //         override.");
  //   }

  //   if (logMode == LogMode::kConsole) {
  //     sink::LogSink *_logSink = new sink::ConsoleLogSink(appId,
  //     appDescription); LoggingMenager *_result = new
  //     LoggingMenager(_logSink, logLevel);

  //     return _result;
  //   } else {
  //     throw std::invalid_argument("The log mode is not currently
  //     supported.");
  //   }
  loger_f_ = std::make_unique<LoggingMenager>(appId, logLevel);
  if (logMode & LogMode::kConsole) {
    AddSink(std::make_unique<sink::ConsoleLogSink>());
  }
  if (logMode & LogMode::kRemote) {
    AddSink(std::make_unique<sink::RemoteLogSink>());
  }
}

void LoggingMenager::AddSink(std::unique_ptr<sink::LogSink> sink_) {
  if (loger_f_ != nullptr) {
    loger_f_->sink_list_.push_back(std::move(sink_));
  }
}

LoggingMenager *LoggingMenager::GetInstance() { return loger_f_.get(); }

LoggingMenager::~LoggingMenager() noexcept {
  for (auto &sin : this->sink_list_) {
    sin.release();
  }
}
}  // namespace log
}  // namespace ara
