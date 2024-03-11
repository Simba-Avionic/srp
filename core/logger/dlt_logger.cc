/**
 * @file dlt_logger.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-02-08
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "core/logger/dlt_logger.h"

#include <time.h>

#include <algorithm>
#include <chrono>  // NOLINT
#include <fstream>
#include <iostream>

#include "communication-core/sockets/socket_config.h"
namespace simba {
namespace core {
namespace logger {
DltLogger::DltLogger(/* args */) {
  // ipc_soc.Init(com::soc::SocketConfig{"", 0, 0});
  this->thread = std::make_unique<std::jthread>(
      [this](std::stop_token token) { this->Loop(token); });
}

void DltLogger::Loop(std::stop_token token) {
  while (!token.stop_requested()) {
    auto data = q.GetWithoutRemove();
    if (ipc_soc.Transmit("SIMBA.DLT", 0, data) == ErrorCode::kOk) {
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

simba::core::ErrorCode DltLogger::Debug(const std::string& log) {
  auto time = this->GetTimeStamp();
  uint8_t* vp = reinterpret_cast<uint8_t*>(&time);
  std::vector<uint8_t> log_vector{0, vp[0], vp[1], vp[2], vp[3]};
  std::copy(this->app_name.begin(), this->app_name.end(),
            std::back_inserter(log_vector));
  std::copy(log.begin(), log.end(), std::back_inserter(log_vector));
  if (!this->q.push(log_vector)) {
    std::cerr << "DLT drop logs!!!" << std::endl;
  }
  return core::ErrorCode::kOk;
}
simba::core::ErrorCode DltLogger::Info(const std::string& log) {
  auto time = this->GetTimeStamp();
  uint8_t* vp = reinterpret_cast<uint8_t*>(&time);
  std::vector<uint8_t> log_vector{1, vp[0], vp[1], vp[2], vp[3]};
  std::copy(this->app_name.begin(), this->app_name.end(),
            std::back_inserter(log_vector));
  std::copy(log.begin(), log.end(), std::back_inserter(log_vector));
  if (!this->q.push(log_vector)) {
    std::cerr << "DLT drop logs!!!" << std::endl;
  }
  return core::ErrorCode::kOk;
}
simba::core::ErrorCode DltLogger::Warning(const std::string& log) {
  auto time = this->GetTimeStamp();
  uint8_t* vp = reinterpret_cast<uint8_t*>(&time);
  std::vector<uint8_t> log_vector{2, vp[0], vp[1], vp[2], vp[3]};
  std::copy(this->app_name.begin(), this->app_name.end(),
            std::back_inserter(log_vector));
  std::copy(log.begin(), log.end(), std::back_inserter(log_vector));
  if (!this->q.push(log_vector)) {
    std::cerr << "DLT drop logs!!!" << std::endl;
  }
  return core::ErrorCode::kOk;
}
simba::core::ErrorCode DltLogger::Error(const std::string& log) {
  auto time = this->GetTimeStamp();
  uint8_t* vp = reinterpret_cast<uint8_t*>(&time);
  std::vector<uint8_t> log_vector{3, vp[0], vp[1], vp[2], vp[3]};
  std::copy(this->app_name.begin(), this->app_name.end(),
            std::back_inserter(log_vector));
  std::copy(log.begin(), log.end(), std::back_inserter(log_vector));
  if (!this->q.push(log_vector)) {
    std::cerr << "DLT drop logs!!!" << std::endl;
  }
  return core::ErrorCode::kOk;
}

uint32_t DltLogger::GetTimeStamp() noexcept {
  timespec t;
  clock_gettime(CLOCK_BOOTTIME, &t);
  return static_cast<uint32_t>(t.tv_sec * 10000 + t.tv_nsec / 100000L);
}

DltLogger::~DltLogger() {}
}  // namespace logger
}  // namespace core
}  // namespace simba
