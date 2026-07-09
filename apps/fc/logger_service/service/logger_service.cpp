/**
 * @file logger_service.cpp
 * @author Krzysztof Kondracki (kondracki.christopher@gmail.com)
 * @brief
 * @version 0.1
 * @date 2026-04-13
 *
 * @copyright Copyright (c) 2026
 *
 */
#include "apps/fc/logger_service/service/logger_service.hpp"

#include <string>
#include <utility>

#include "ara/log/log.h"
#include "apps/fc/logger_service/service/logger_builder.hpp"
#include "core/common/condition.h"
#include "core/csvdriver/csvdriver.h"
#include "core/time/sys_time.hpp"

namespace srp {
namespace logger {

namespace {
  static constexpr auto kLoggerFilename = "_fc_log.csv";
  static constexpr auto kLoggerFilenamePrefix = "/home/root/";
  static constexpr std::uint16_t kSaveIntervalMs = 10;
  static constexpr auto kEnvServicePathName = "srp/apps/FcFileLoggerApp/envServiceFc_ipc";
  static constexpr auto kApogeeServocePathName = "srp/apps/ApogeeDetectService/ApogeeDetectService";
  static constexpr auto kMainServicePathName = "srp/apps/MainService/MainService";
  static constexpr auto kGpsServicePathName = "srp/apps/GPSService/GPSService";
  static constexpr auto kRadioServicePathName = "srp/apps/FcFileLoggerApp/RadioApp";
  static constexpr auto kUdpServicePathName = "srp/apps/FcFileLoggerApp/logService_udp";
  static constexpr auto kIpcServicePathName = "srp/apps/FcFileLoggerApp/logService_ipc";
  static constexpr auto kSysStatServicePathName = "srp/apps/FcFileLoggerApp/FcSysStatService_ipc";
  static constexpr auto kFileDidPathName = "/srp/apps/FcFileLoggerApp/logger_did";
  static constexpr std::uint8_t kLogsOn = 1;
  static constexpr std::uint8_t kLogsOff = 0;
}  // namespace

LoggerService::LoggerService()
    : env_service_proxy_{ara::core::InstanceSpecifier{kEnvServicePathName}},
      stat_service_proxy_{ara::core::InstanceSpecifier{kSysStatServicePathName}},
      apogee_proxy_{ara::core::InstanceSpecifier{kApogeeServocePathName}},
      main_proxy_{ara::core::InstanceSpecifier{kMainServicePathName}},
      gps_proxy_{ara::core::InstanceSpecifier{kGpsServicePathName}},
      radio_proxy_{ara::core::InstanceSpecifier{kRadioServicePathName}},
      radio_handler_{nullptr},
      gps_handler_{nullptr},
      env_service_handler_{nullptr},
      stat_service_handler_{nullptr},
      main_handler_{nullptr},
      apogee_handler_{nullptr},
      did_instance_{kFileDidPathName},
      timestamp_{std::make_shared<core::timestamp::TimestampController>()},
      save_thread_{nullptr} {
  Builder builder([this](std::uint8_t status) { StartFuncHandler(status); });
  auto result = builder.SetLoggerDID(did_instance_)
                    .SetLoggerIPC(kIpcServicePathName)
                    .SetLoggerUDP(kUdpServicePathName)
                    .Build();
  logger_did_ = std::move(result.logger_did);
  service_ipc_ = std::move(result.service_ipc);
  service_udp_ = std::move(result.service_udp);

  if (!timestamp_->Init()) {
    ara::log::LogError() << "LoggerService: failed to initialize timestamp controller";
  }
}

LoggerService::~LoggerService() {
  StartFuncHandler(0);
}

int LoggerService::Initialize(
    const std::map<ara::core::StringView, ara::core::StringView> /*parms*/) {
  logger_did_->Offer();
  service_ipc_->StartOffer();
  service_udp_->StartOffer();
  SomeIpInit();
  return 0;
}

int LoggerService::Run(const std::stop_token& token) {
  StartFuncHandler(1);
  while (!token.stop_requested()) {
    service_ipc_->LoggingState.Update(save_state_.load());
    service_udp_->LoggingState.Update(save_state_.load());
    core::condition::wait_for(std::chrono::milliseconds(1000), token);
  }
  StartFuncHandler(0);
  service_ipc_->StopOffer();
  service_udp_->StopOffer();
  logger_did_->StopOffer();
  return 0;
}

void LoggerService::StartFuncHandler(std::uint8_t status) {
  if (status == kLogsOn && !save_thread_) {
    save_thread_ = std::make_shared<std::jthread>(
        [this](std::stop_token token) { SaveLoop(token); });
    return;
  }

  if (status == kLogsOff && save_thread_) {
    save_thread_->request_stop();
    save_thread_->join();
    save_thread_.reset();
  }
}

void LoggerService::SaveLoop(const std::stop_token& token) {
  csv::CSVDriver writer;
  writer.Init(std::make_unique<core::FileHandler>());

  const auto prefix = core::time::TimeChanger::ReadSystemTimeAsString();
  const std::string filename = kLoggerFilenamePrefix + prefix.value_or("") + kLoggerFilename;
  if (writer.Open(filename, data_.get_header()) != 0) {
    ara::log::LogError() << "LoggerService::SaveLoop: failed to open file " << filename;
    return;
  }

  save_state_.store(kLogsOn);
  while (!token.stop_requested()) {
    const auto start = std::chrono::high_resolution_clock::now();
    auto ts = timestamp_->GetNewTimeStamp();
    if (ts.has_value() && writer.WriteLine(data_.to_string(std::to_string(ts.value()))) != 0) {
      ara::log::LogWarn() << "LoggerService::SaveLoop: failed to write line";
    }

    const auto now = std::chrono::high_resolution_clock::now();
    const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start);
    if (elapsed < std::chrono::milliseconds(kSaveIntervalMs)) {
      core::condition::wait_for(std::chrono::milliseconds(kSaveIntervalMs) - elapsed, token);
    }
  }

  writer.Close();
  save_state_.store(kLogsOff);
}

void LoggerService::SomeIpInit() {
  radio_proxy_.StartFindService([this](auto handler) {
    radio_handler_ = handler;
    radio_handler_->RadioStatusEvent.Subscribe(1, [this](std::uint8_t /*status*/) {
      radio_handler_->RadioStatusEvent.SetReceiveHandler([this]() {
        const auto res_opt = radio_handler_->RadioStatusEvent.GetNewSamples();
        if (!res_opt.HasValue()) {
          return;
        }
        const auto res = res_opt.Value();
        data_.SetRadioStatus(res.rxerrors, res.fixed, res.rssi, res.remrssi,
                             res.txbuf, res.noise, res.remnoise);
      });
    });
  });
  gps_proxy_.StartFindService([this](auto handler) {
    gps_handler_ = handler;
    gps_handler_->GPSStatusEvent.Subscribe(1, [this](std::uint8_t status) {
      gps_handler_->GPSStatusEvent.SetReceiveHandler([this]() {
        const auto res_opt = gps_handler_->GPSStatusEvent.GetNewSamples();
        if (!res_opt.HasValue()) {
          return;
        }
        const auto res = res_opt.Value();
        data_.SetGpsData(res.latitude, res.longitude, res.altitude);
      });
    });
  });
  main_proxy_.StartFindService([this](auto handler) {
    main_handler_ = handler;
    main_handler_->CurrentModeStatusEvent.Subscribe(1, [this](std::uint8_t statuc) {
      main_handler_->CurrentModeStatusEvent.SetReceiveHandler([this]() {
        const auto res_opt = main_handler_->CurrentModeStatusEvent.GetNewSamples();
        if (!res_opt.HasValue()) {
          return;
        }
        data_.SetFCMode(res_opt.Value());
      });
    });
  });
  apogee_proxy_.StartFindService([this](auto handler) {
    apogee_handler_ = handler;
    apogee_handler_->newApogeeDetected.Subscribe(1, [this](std::uint8_t status) {
      apogee_handler_->newApogeeDetected.SetReceiveHandler([this]() {
        auto res_opt = apogee_handler_->newApogeeDetected.GetNewSamples();
        if (!res_opt.HasValue()) {
          return;
        }
        data_.SetApogeeDetected(res_opt.Value());
      });
    });
    apogee_handler_->newMainParachuteDetected.Subscribe(1, [this](std::uint8_t status) {
      apogee_handler_->newMainParachuteDetected.SetReceiveHandler([this]() {
        auto res = apogee_handler_->newMainParachuteDetected.GetNewSamples();
        if (!res.HasValue()) {
          return;
        }
        data_.SetMainParachuteDetected(res.Value());
      });
    });
  });
  stat_service_proxy_.StartFindService([this](auto handler) {
    stat_service_handler_ = handler;
    stat_service_handler_->NewSystemUsage.Subscribe(1, [this](std::uint8_t /*status*/) {
      stat_service_handler_->NewSystemUsage.SetReceiveHandler([this]() {
        auto res_opt = stat_service_handler_->NewSystemUsage.GetNewSamples();
        if (!res_opt.HasValue()) {
          return;
        }
        const auto res = res_opt.Value();
        data_.SetSystemUsage(res.cpu_usage, res.mem_usage, res.disk_utilization);
      });
    });
  });

  env_service_proxy_.StartFindService([this](auto handler) {
    env_service_handler_ = handler;

    env_service_handler_->newIMUEvent.Subscribe(1, [this](std::uint8_t status) {
      env_service_handler_->newIMUEvent.SetReceiveHandler([this]() {
        const auto res_opt = env_service_handler_->newIMUEvent.GetNewSamples();
        if (!res_opt.HasValue()) {
          return;
        }
        const auto res = res_opt.Value();
        data_.SetIMU(res.accel_x, res.accel_y, res.accel_z,
                                        res.gyroscope_x, res.gyroscope_y, res.gyroscope_z);
      });
    });
    env_service_handler_->newBoardTempEvent_1.Subscribe(1, [this](std::uint8_t /*status*/) {
      env_service_handler_->newBoardTempEvent_1.SetReceiveHandler([this]() {
        auto res = env_service_handler_->newBoardTempEvent_1.GetNewSamples();
        if (res.HasValue()) {
          data_.SetBoardTemp1(res.Value());
        }
      });
    });

    env_service_handler_->newBoardTempEvent_2.Subscribe(1, [this](std::uint8_t /*status*/) {
      env_service_handler_->newBoardTempEvent_2.SetReceiveHandler([this]() {
        auto res = env_service_handler_->newBoardTempEvent_2.GetNewSamples();
        if (res.HasValue()) {
          data_.SetBoardTemp2(res.Value());
        }
      });
    });

    env_service_handler_->newBoardTempEvent_3.Subscribe(1, [this](std::uint8_t /*status*/) {
      env_service_handler_->newBoardTempEvent_3.SetReceiveHandler([this]() {
        auto res = env_service_handler_->newBoardTempEvent_3.GetNewSamples();
        if (res.HasValue()) {
          data_.SetBoardTemp3(res.Value());
        }
      });
    });

    env_service_handler_->newBME280Event.Subscribe(1, [this](std::uint8_t /*status*/) {
      env_service_handler_->newBME280Event.SetReceiveHandler([this]() {
        const auto res_opt = env_service_handler_->newBME280Event.GetNewSamples();
        if (!res_opt.HasValue()) {
          return;
        }
        const auto res = res_opt.Value();
        data_.SetBmeData(res.temperature, res.humidity, res.altitude);
      });
    });
  });
}

}  // namespace logger
}  // namespace srp
