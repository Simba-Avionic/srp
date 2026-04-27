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
#include "ara/log/logging_menager.h"
#include "core/time/sys_time.hpp"
#include "core/csvdriver/csvdriver.h"
#include "apps/ec/logger_service/service/logger_builder.hpp"


namespace srp {
namespace logger {

namespace {
  static constexpr std::string kloger_filename = "_log.csv";
  static constexpr std::string kloger_filename_prefix = "/home/root/";
  static constexpr uint16_t kSave_interval = 5;
  static constexpr auto kEnv_service_path_name = "srp/apps/FileLoggerApp/EnvApp";
  static constexpr auto kUdp_service_path_name = "srp/apps/FileLoggerApp/logService_udp";
  static constexpr auto kIpc_service_path_name = "srp/apps/FileLoggerApp/logService_ipc";
  static constexpr auto kSysStat_service_path_name = "srp/apps/FileLoggerApp/SysStatService";
  static constexpr auto kPrimer_service_path_name = "srp/apps/FileLoggerApp/PrimerService";
  static constexpr auto kServo_service_path_name = "srp/apps/FileLoggerApp/ServoService";
  static constexpr auto kEngine_service_path_name = "srp/apps/FileLoggerApp/EngineService";
  static constexpr auto kFile_did_path_name = "/srp/apps/FileLoggerApp/logger_did";
  static constexpr auto kLogs_on = 1;
  static constexpr auto kLogs_off = 0;
  static constexpr auto kHeartBeatPinID = 2;
}  // namespace
void LoggerService::SaveLoop(const std::stop_token& token,
            std::shared_ptr<core::timestamp::TimestampController> timestamp) {
  csv::CSVDriver writer_;
  writer_.Init(std::make_unique<core::FileHandler>());

  auto prefix = core::time::TimeChanger::ReadSystemTimeAsString();
  std::string filename;
  filename = kloger_filename_prefix + prefix.value_or("") + kloger_filename;

  if (writer_.Open(filename, data.get_header()) != 0) {
    ara::log::LogError() << "LoggerService::SaveLoop: Failed to open file: " << filename;
    return;
  }

  ara::log::LogInfo() << "LoggerService::SaveLoop: Started logging to " << filename;
  save_state.store(kLogs_on);

  try {
    while (!token.stop_requested()) {
      const auto start = std::chrono::high_resolution_clock::now();
      auto val = timestamp->GetNewTimeStamp();
      if (!val.has_value()) {
        continue;
      }
      if (writer_.WriteLine(this->data.to_string(std::to_string(val.value()))) != 0) {
        ara::log::LogWarn() << "LoggerService::SaveLoop: Failed to write line";
      }

      const auto now = std::chrono::high_resolution_clock::now();
      const auto elapsed = std::chrono::duration_cast<
                  std::chrono::milliseconds>(now - start).count();
      if (elapsed < kSave_interval) {
        core::condition::wait_for(std::chrono::milliseconds(kSave_interval - elapsed), token);
      }
    }
  } catch (...) {
    writer_.Close();
    save_state.store(kLogs_off);
    ara::log::LogFatal() << "LoggerService::SaveLoop: Stopped logging due to Fatal error";
  }

  writer_.Close();
  save_state.store(kLogs_off);
  ara::log::LogInfo() << "LoggerService::SaveLoop: Stopped logging, file closed";
}

int LoggerService::Run(const std::stop_token& token) {
  while (!token.stop_requested()) {
    if (gpio_.SetPinValue(kHeartBeatPinID, 1, 500) != core::ErrorCode::kOk) {
      ara::log::LogWarn() << "EngineApp::Run: Failed to toggle heartbeat pin";
    }
    service_ipc->LoggingState.Update(save_state.load());
    service_udp->LoggingState.Update(save_state.load());
    core::condition::wait_for(std::chrono::milliseconds(1000), token);
  }
  service_ipc->StopOffer();
  service_udp->StopOffer();
  logger_did_->StopOffer();
  return 0;
}

int LoggerService::Initialize(const std::map<ara::core::StringView, ara::core::StringView>
                    parms) {
  logger_did_->Offer();
  service_ipc->StartOffer();
  service_udp->StartOffer();
  this->SomeIpInit();
  return 0;
}

LoggerService::~LoggerService() {}

LoggerService::LoggerService():
      someip_logger{ara::log::LoggingMenager::GetInstance()->CreateLogger(
          "SOME", "", ara::log::LogLevel::kDebug)},
      env_service_proxy{ara::core::InstanceSpecifier{kEnv_service_path_name}},
      stat_service_proxy{ara::core::InstanceSpecifier{kSysStat_service_path_name}},
      primer_service_proxy{ara::core::InstanceSpecifier{kPrimer_service_path_name}},
      servo_service_proxy{ara::core::InstanceSpecifier{kServo_service_path_name}},
      engine_service_proxy{ara::core::InstanceSpecifier{kEngine_service_path_name}},
      env_service_handler{nullptr},
      stat_service_handler{nullptr},
      primer_service_handler{nullptr},
      servo_service_handler{nullptr},
      engine_service_handler{nullptr},
      did_instance{kFile_did_path_name},
      timestamp_{std::make_shared<core::timestamp::TimestampController>()},
      save_thread_{nullptr} {
  auto builder = Builder([this](uint8_t status) { this->start_func_handler(status); });
  auto result = builder.setLoggerDID(did_instance)
                .setLoggerIPC(kIpc_service_path_name)
                .setLoggerUDP(kUdp_service_path_name)
                .build();

  this->logger_did_ = std::move(result.loggerDID);
  this->service_ipc = std::move(result.serviceIPC);
  this->service_udp = std::move(result.serviceUDP);
  if (!timestamp_->Init()) {
    ara::log::LogError() << "LoggerService::LoggerService: Failed to initialize timestamp controller";
    return;
  }
}

void LoggerService::start_func_handler(const std::uint8_t status) {
  ara::log::LogWarn() << "LoggerService::start_func_handler: status=" << static_cast<unsigned>(status)
                      << " save_thread_=" << (this->save_thread_ ? "set" : "null");
  if (status == 1 && !this->save_thread_) {
    this->save_thread_ = std::make_shared<std::jthread>(
      [this](std::stop_token token) {
          SaveLoop(token, this->timestamp_);
      });
  } else if (status == 0 && this->save_thread_) {
    if (this->save_thread_) {
      this->save_thread_->request_stop();
      this->save_thread_->join();
      this->save_thread_.reset();
      }
  }
}

void LoggerService::SomeIpInit() {
  someip_logger.LogDebug() << "SomeIpInit started";
  this->engine_service_proxy.StartFindService([this](auto handler) {
    someip_logger.LogDebug() << "Engine service handler discovered";
    this->engine_service_handler = handler;
    engine_service_handler->CurrentMode.Subscribe(1, [this](const uint8_t status) {
      someip_logger.LogDebug() << "Subscribed to Engine CurrentMode, status="
                               << std::to_string(static_cast<int>(status));
      engine_service_handler->CurrentMode.SetReceiveHandler([this] () {
        auto res = engine_service_handler->CurrentMode.GetNewSamples();
        if (!res.HasValue()) {
          return;
        }
        someip_logger.LogDebug() << "Engine CurrentMode sample: "
                                 << std::to_string(res.Value());
        this->data.SetEngineMode(res.Value());
      });
    });
    engine_service_handler->NewVentValveStatus.Subscribe(1, [this](const uint8_t status) {
      someip_logger.LogDebug() << "Subscribed to Engine NewVentValveStatus, status="
                               << std::to_string(static_cast<int>(status));
      engine_service_handler->NewVentValveStatus.SetReceiveHandler([this] () {
        auto res = engine_service_handler->NewVentValveStatus.GetNewSamples();
        if (!res.HasValue()) {
          return;
        }
        someip_logger.LogDebug() << "Engine NewVentValveStatus sample: "
                                 << std::to_string(res.Value());
        this->data.SetNewVentValveStatus(res.Value());
      });
    });
  });
  this->servo_service_proxy.StartFindService([this](auto handler) {
    someip_logger.LogDebug() << "Servo service handler discovered";
    this->servo_service_handler = handler;
    servo_service_handler->ServoStatusEvent.Subscribe(1, [this](const uint8_t status) {
      someip_logger.LogDebug() << "Subscribed to ServoStatusEvent, status="
                               << std::to_string(static_cast<int>(status));
      servo_service_handler->ServoStatusEvent.SetReceiveHandler([this] () {
        auto res = servo_service_handler->ServoStatusEvent.GetNewSamples();
        if (!res.HasValue()) {
          return;
        }
        someip_logger.LogDebug() << "ServoStatusEvent sample: "
                                 << std::to_string(res.Value());
        this->data.SetServoStatus(res.Value());
      });
    });
    servo_service_handler->ServoDumpStatusEvent.Subscribe(1, [this](const uint8_t status) {
      someip_logger.LogDebug() << "Subscribed to ServoDumpStatusEvent, status="
                               << std::to_string(static_cast<int>(status));
      servo_service_handler->ServoDumpStatusEvent.SetReceiveHandler([this] () {
        auto res = servo_service_handler->ServoDumpStatusEvent.GetNewSamples();
        if (!res.HasValue()) {
          return;
        }
        someip_logger.LogDebug() << "ServoDumpStatusEvent sample: "
                                 << std::to_string(res.Value());
        this->data.SetServoDumpStatus(res.Value());
      });
    });
    servo_service_handler->ServoVentStatusEvent.Subscribe(1, [this](const uint8_t status){
      someip_logger.LogDebug() << "Subscribed to ServoVentStatusEvent, status="
                               << std::to_string(static_cast<int>(status));
      servo_service_handler->ServoVentStatusEvent.SetReceiveHandler([this] () {
        auto res = servo_service_handler->ServoVentStatusEvent.GetNewSamples();
        if (!res.HasValue()) {
          return;
        }
        someip_logger.LogDebug() << "ServoVentStatusEvent sample: "
                                 << std::to_string(res.Value());
        this->data.SetServoVentStatus(res.Value());
      });
    });
  });
  this->primer_service_proxy.StartFindService([this](auto handler) {
    someip_logger.LogDebug() << "Primer service handler discovered";
    this->primer_service_handler = handler;
    primer_service_handler->primeStatusEvent.Subscribe(1, [this](const uint8_t status) {
      someip_logger.LogDebug() << "Subscribed to Primer primeStatusEvent, status="
                               << std::to_string(static_cast<int>(status));
      primer_service_handler->primeStatusEvent.SetReceiveHandler([this] () {
        auto res = primer_service_handler->primeStatusEvent.GetNewSamples();
        if (!res.HasValue()) {
          return;
        }
        someip_logger.LogDebug() << "Primer primeStatusEvent sample: "
                                 << std::to_string(res.Value());
        this->data.SetPrimerStatus(res.Value());
      });
    });
  });
  this->stat_service_proxy.StartFindService([this](auto handler) {
    someip_logger.LogDebug() << "SysStat service handler discovered";
    this->stat_service_handler = handler;
    stat_service_handler->NewSystemUsage.Subscribe(1, [this](const uint8_t status) {
      someip_logger.LogDebug() << "Subscribed to SysStat NewSystemUsage, status="
                               << std::to_string(static_cast<int>(status));
      stat_service_handler->NewSystemUsage.SetReceiveHandler([this] () {
        auto res = stat_service_handler->NewSystemUsage.GetNewSamples();
        if (!res.HasValue()) {
          return;
        }
        someip_logger.LogDebug() << "SysStat NewSystemUsage sample: cpu="
                                 << std::to_string(res.Value().cpu_usage)
                                 << ", mem=" << std::to_string(res.Value().mem_usage)
                                 << ", disk=" << std::to_string(res.Value().disk_utilization);
        this->data.SetSysStatus(res.Value());
      });
    });
  });
  this->env_service_proxy.StartFindService([this](auto handler) {
    someip_logger.LogDebug() << "Env service handler discovered";
    this->env_service_handler = handler;
    env_service_handler->newTempEvent_1.Subscribe(1, [this](const uint8_t status) {
      someip_logger.LogDebug() << "Subscribed to Env newTempEvent_1, status="
                               << std::to_string(static_cast<int>(status));
      env_service_handler->newTempEvent_1.SetReceiveHandler([this] () {
        auto res = env_service_handler->newTempEvent_1.GetNewSamples();
        if (!res.HasValue()) {
          return;
        }
        someip_logger.LogDebug() << "Env newTempEvent_1 sample: "
                                 << std::to_string(res.Value());
        this->data.SetTemp1(res.Value());
      });
    });
    env_service_handler->newTempEvent_2.Subscribe(1, [this](const uint8_t status) {
      someip_logger.LogDebug() << "Subscribed to Env newTempEvent_2, status="
                               << std::to_string(static_cast<int>(status));
      env_service_handler->newTempEvent_2.SetReceiveHandler([this] () {
        auto res = env_service_handler->newTempEvent_2.GetNewSamples();
        if (!res.HasValue()) {
          return;
        }
        someip_logger.LogDebug() << "Env newTempEvent_2 sample: "
                                 << std::to_string(res.Value());
        this->data.SetTemp2(res.Value());
      });
    });
    env_service_handler->newTempEvent_3.Subscribe(1, [this](const uint8_t status) {
      someip_logger.LogDebug() << "Subscribed to Env newTempEvent_3, status="
                               << std::to_string(static_cast<int>(status));
      env_service_handler->newTempEvent_3.SetReceiveHandler([this] () {
        auto res = env_service_handler->newTempEvent_3.GetNewSamples();
        if (!res.HasValue()) {
          return;
        }
        someip_logger.LogDebug() << "Env newTempEvent_3 sample: "
                                 << std::to_string(res.Value());
        this->data.SetTemp3(res.Value());
      });
    });
    env_service_handler->newBoardTempEvent1.Subscribe(1, [this](const uint8_t status) {
      someip_logger.LogDebug() << "Subscribed to Env newBoardTempEvent1, status="
                               << std::to_string(static_cast<int>(status));
      env_service_handler->newBoardTempEvent1.SetReceiveHandler([this] () {
        auto res = env_service_handler->newBoardTempEvent1.GetNewSamples();
        if (!res.HasValue()) {
          return;
        }
        someip_logger.LogDebug() << "Env newBoardTempEvent1 sample: "
                                 << std::to_string(res.Value());
        this->data.SetBoardTemp1(res.Value());
      });
    });
    env_service_handler->newBoardTempEvent2.Subscribe(1, [this](const uint8_t status) {
      someip_logger.LogDebug() << "Subscribed to Env newBoardTempEvent2, status="
                               << std::to_string(static_cast<int>(status));
      env_service_handler->newBoardTempEvent2.SetReceiveHandler([this] () {
        auto res = env_service_handler->newBoardTempEvent2.GetNewSamples();
        if (!res.HasValue()) {
          return;
        }
        someip_logger.LogDebug() << "Env newBoardTempEvent2 sample: "
                                 << std::to_string(res.Value());
        this->data.SetBoardTemp2(res.Value());
      });
    });
    env_service_handler->newBoardTempEvent3.Subscribe(1, [this](const uint8_t status) {
      someip_logger.LogDebug() << "Subscribed to Env newBoardTempEvent3, status="
                               << std::to_string(static_cast<int>(status));
      env_service_handler->newBoardTempEvent3.SetReceiveHandler([this] () {
        auto res = env_service_handler->newBoardTempEvent3.GetNewSamples();
        if (!res.HasValue()) {
          return;
        }
        someip_logger.LogDebug() << "Env newBoardTempEvent3 sample: "
                                 << std::to_string(res.Value());
        this->data.SetBoardTemp3(res.Value());
      });
    });
    env_service_handler->newDPressEvent.Subscribe(1, [this](const uint8_t status) {
      someip_logger.LogDebug() << "Subscribed to Env newDPressEvent, status="
                               << std::to_string(static_cast<int>(status));
      env_service_handler->newDPressEvent.SetReceiveHandler([this] () {
        auto res = env_service_handler->newDPressEvent.GetNewSamples();
        if (!res.HasValue()) {
          return;
        }
        someip_logger.LogDebug() << "Env newDPressEvent sample: "
                                 << std::to_string(res.Value());
        this->data.SetTankDPress(res.Value());
      });
    });
    env_service_handler->newPressEvent.Subscribe(1, [this](const uint8_t status) {
      someip_logger.LogDebug() << "Subscribed to Env newPressEvent, status="
                               << std::to_string(static_cast<int>(status));
      env_service_handler->newPressEvent.SetReceiveHandler([this] () {
        auto res = env_service_handler->newPressEvent.GetNewSamples();
        if (!res.HasValue()) {
          return;
        }
        someip_logger.LogDebug() << "Env newPressEvent sample: "
                                 << std::to_string(res.Value());
        this->data.SetTankPress(res.Value());
      });
    });
    env_service_handler->newTensoEvent.Subscribe(1, [this](const uint8_t status) {
      someip_logger.LogDebug() << "Subscribed to Env newTensoEvent, status="
                               << std::to_string(static_cast<int>(status));
      env_service_handler->newTensoEvent.SetReceiveHandler([this] () {
        auto res = env_service_handler->newTensoEvent.GetNewSamples();
        if (!res.HasValue()) {
          return;
        }
        someip_logger.LogDebug() << "Env newTensoEvent sample: "
                                 << std::to_string(res.Value());
        this->data.SetTenso(res.Value());
      });
    });
  });
}

}  // namespace logger
}  // namespace srp
