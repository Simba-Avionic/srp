/**
 * @file remote_log_sink.cpp
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-09-01
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "ara/log/sink/remote_log_sink.h"

#include <algorithm>
#include <iostream>

namespace ara {
namespace log {
namespace sink {
RemoteLogSink::RemoteLogSink() {
  this->thread = std::make_unique<std::jthread>(
      [this](std::stop_token token) { this->Loop(token); });
}

void RemoteLogSink::Log(const LogStream& logStream) {
  auto time = this->GetTimeStamp();
  uint8_t* vp = reinterpret_cast<uint8_t*>(&time);
  std::vector<uint8_t> log_vector{};
  switch (logStream.GetLogLevel()) {
    case ara::log::LogLevel::kVerbose:
      log_vector.push_back(0U);
      break;
    case ara::log::LogLevel::kDebug:
      log_vector.push_back(1U);
      break;
    case ara::log::LogLevel::kInfo:
      log_vector.push_back(2U);
      break;
    case ara::log::LogLevel::kWarn:
      log_vector.push_back(3U);
      break;
    case ara::log::LogLevel::kError:
      log_vector.push_back(4U);
      break;
    case ara::log::LogLevel::kFatal:
      log_vector.push_back(5U);
      break;
    default:
      log_vector.push_back(0U);
      break;
  }
  log_vector.push_back(vp[0]);
  log_vector.push_back(vp[1]);
  log_vector.push_back(vp[2]);
  log_vector.push_back(vp[3]);
  const auto& app_name = logStream.GetAppId();
  const auto& ctx_name = logStream.GetCtx();
  const auto& slog = logStream.ToString();
  std::copy(app_name.begin(), app_name.end(), std::back_inserter(log_vector));
  std::copy(ctx_name.begin(), ctx_name.end(), std::back_inserter(log_vector));
  std::copy(slog.begin(), slog.end(), std::back_inserter(log_vector));
  if (!this->q.push(log_vector)) {
    std::cerr << "DLT drop logs!!!" << std::endl;
  }
}

void RemoteLogSink::Loop(std::stop_token token) {
  while (!token.stop_requested()) {
    auto data = q.GetWithoutRemove(token);
    if (data.has_value()) {
      if (ipc_soc.Transmit("SIMBA.ARA.DLT", 0, data.value()) ==
          simba::core::ErrorCode::kOk) {
        q.Remove();
        drop_number = 0;
      } else {
        std::this_thread::sleep_for(std::chrono::seconds{1});
        drop_number++;
        if (drop_number > 20) {
          drop_number = 0;
          q.Remove();
          std::cerr << "DLT drop logs!!!" << std::endl;
        }
      }
    }
  }
}
RemoteLogSink::~RemoteLogSink() {
  if (this->thread->get_stop_source().stop_possible())
    this->thread->request_stop();
  if (this->thread->joinable()) this->thread->join();
}
}  // namespace sink
}  // namespace log
}  // namespace ara
