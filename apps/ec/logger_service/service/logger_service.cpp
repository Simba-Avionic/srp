/**
 * @file logger_service.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-22
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <utility>
#include "apps/ec/logger_service/service/logger_service.hpp"
#include "ara/exec/adaptive_application.h"
#include "core/common/condition.h"
#include "ara/log/log.h"
#include "core/time/sys_time.hpp"
#include "apps/ec/logger_service/service/logger_builder.hpp"


namespace srp {
namespace logger {

namespace {
  constexpr auto kCsv_header = "TIMESTAMP;TEMP1;TEMP2;TEMP3;TANK_PRESS;TANK_D_PRESS";
  constexpr std::string kCsv_filename = "log.csv";
  constexpr std::string kCsv_filename_prefix = "/home/root/";
  constexpr uint16_t kSave_interval = 100;
  constexpr auto k_save_interval_fix = 1;
  constexpr auto kEnv_service_path_name = "srp/apps/FileLoggerApp/EnvApp";
  constexpr auto kUdp_service_path_name = "srp/apps/FileLoggerApp/logService_udp";
  constexpr auto kIpc_service_path_name = "srp/apps/FileLoggerApp/logService_ipc";
  constexpr auto kFile_did_path_name = "/srp/apps/FileLoggerApp/logger_did";
}  // namespace


void LoggerService::SaveLoop(const std::stop_token& token) {
  csv::CSVDriver csv_;
  core::timestamp::TimestampController timestamp_;
  auto prefix = core::time::TimeChanger::ReadSystemTimeAsString();
  if (!prefix.has_value()) {
    csv_.Open(kCsv_filename_prefix + kCsv_filename, kCsv_header);
  } else {
    csv_.Open(kCsv_filename_prefix + prefix.value() + kCsv_filename, kCsv_header);
  }
  timestamp_.Init();
  while (!token.stop_requested()) {
    const auto start = std::chrono::high_resolution_clock::now();
    auto val = timestamp_.GetNewTimeStamp();
    if (!val.has_value()) {
      continue;
    }
    csv_.WriteLine(this->data.to_string(std::to_string(val.value())));
    const auto now = std::chrono::high_resolution_clock::now();
    const auto elapsed = std::chrono::duration_cast<
                std::chrono::milliseconds>(now - start).count() +  k_save_interval_fix;
    core::condition::wait_for(std::chrono::milliseconds(kSave_interval - elapsed), token);
  }
  csv_.Close();
}

int LoggerService::Run(const std::stop_token& token) {
  core::condition::wait(token);
  logger_did_->StopOffer();
  return 0;
}

int LoggerService::Initialize(const std::map<ara::core::StringView, ara::core::StringView>
                    parms) {
  this->SomeIpInit();
  return 0;
}

LoggerService::~LoggerService() {}

LoggerService::LoggerService():
    env_service_proxy{ara::core::InstanceSpecifier{kEnv_service_path_name}},
    env_service_handler{nullptr}, save_thread_{nullptr},
    did_instance{kFile_did_path_name} {
  auto builder = Builder([this](uint8_t status) { this->start_func_handler(status); });
  auto result = builder.setLoggerDID(did_instance)
                .setLoggerIPC(kIpc_service_path_name)
                .setLoggerUDP(kUdp_service_path_name)
                .build();

  this->logger_did_ = std::move(result.loggerDID);
  this->service_ipc = std::move(result.serviceIPC);
  this->service_udp = std::move(result.serviceUDP);
  logger_did_->Offer();
}

void LoggerService::start_func_handler(const std::uint8_t status) {
  if (status == 1 && !this->save_thread_) {
      this->save_thread_ = std::make_unique<std::jthread>(
          [this](std::stop_token token) {
              SaveLoop(token);
          });
  } else if (status == 0 && this->save_thread_) {
      this->save_thread_.reset();
  }
}


void LoggerService::SomeIpInit() {
  this->env_service_proxy.StartFindService([this](auto handler) {
    this->env_service_handler = handler;
    env_service_handler->newTempEvent_1.Subscribe(1, [this](const uint8_t status) {
      env_service_handler->newTempEvent_1.SetReceiveHandler([this] () {
        auto res = env_service_handler->newTempEvent_1.GetNewSamples();
        if (!res.HasValue()) {
          return;
        }
        this->data.SetTemp1(res.Value());
      });
    });
    env_service_handler->newTempEvent_2.Subscribe(1, [this](const uint8_t status) {
      env_service_handler->newTempEvent_2.SetReceiveHandler([this] () {
        auto res = env_service_handler->newTempEvent_2.GetNewSamples();
        if (!res.HasValue()) {
          return;
        }
        this->data.SetTemp2(res.Value());
      });
    });
    env_service_handler->newTempEvent_3.Subscribe(1, [this](const uint8_t status) {
      env_service_handler->newTempEvent_3.SetReceiveHandler([this] () {
        auto res = env_service_handler->newTempEvent_3.GetNewSamples();
        if (!res.HasValue()) {
          return;
        }
        this->data.SetTemp3(res.Value());
      });
    });
    env_service_handler->newDPressEvent.Subscribe(1, [this](const uint8_t status) {
      env_service_handler->newDPressEvent.SetReceiveHandler([this] () {
        auto res = env_service_handler->newDPressEvent.GetNewSamples();
        if (!res.HasValue()) {
          return;
        }
        this->data.SetTankDPress(res.Value());
      });
    });
    env_service_handler->newPressEvent.Subscribe(1, [this](const uint8_t status) {
      env_service_handler->newPressEvent.SetReceiveHandler([this] () {
        auto res = env_service_handler->newPressEvent.GetNewSamples();
        if (!res.HasValue()) {
          return;
        }
        this->data.SetTankPress(res.Value());
      });
    });
  });
}

}  // namespace logger
}  // namespace srp

