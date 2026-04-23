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
      env_service_proxy{ara::core::InstanceSpecifier{kEnv_service_path_name}},
      stat_service_proxy{ara::core::InstanceSpecifier{kSysStat_service_path_name}},
      primer_service_proxy{ara::core::InstanceSpecifier{kPrimer_service_path_name}},
      servo_service_proxy{ara::core::InstanceSpecifier{kServo_service_path_name}},
      engine_service_proxy{ara::core::InstanceSpecifier{kEngine_service_path_name}},
      env_service_handler{nullptr},
      save_thread_{nullptr},
      stat_service_handler{nullptr},
      primer_service_handler{nullptr},
      servo_service_handler{nullptr},
      engine_service_handler{nullptr},
      did_instance{kFile_did_path_name},
      timestamp_{std::make_shared<core::timestamp::TimestampController>()} {
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
  this->engine_service_proxy.StartFindService([this](auto handler) {
    this->engine_service_handler = handler;
    engine_service_handler->CurrentMode.Subscribe(1, [this](const uint8_t status) {
      engine_service_handler->CurrentMode.SetReceiveHandler([this] () {
        auto res = engine_service_handler->CurrentMode.GetNewSamples();
        if (!res.HasValue()) {
          return;
        }
        this->data.SetEngineMode(res.Value());
      });
    });
    engine_service_handler->NewVentValveStatus.Subscribe(1, [this](const uint8_t status) {
      engine_service_handler->NewVentValveStatus.SetReceiveHandler([this] () {
        auto res = engine_service_handler->NewVentValveStatus.GetNewSamples();
        if (!res.HasValue()) {
          return;
        }
        this->data.SetNewVentValveStatus(res.Value());
      });
    });
  });
  this->servo_service_proxy.StartFindService([this](auto handler) {
    this->servo_service_handler = handler;
    servo_service_handler->ServoStatusEvent.Subscribe(1, [this](const uint8_t status) {
      servo_service_handler->ServoStatusEvent.SetReceiveHandler([this] () {
        auto res = servo_service_handler->ServoStatusEvent.GetNewSamples();
        if (!res.HasValue()) {
          return;
        }
        this->data.SetServoStatus(res.Value());
      });
    });
    servo_service_handler->ServoDumpStatusEvent.Subscribe(1, [this](const uint8_t status) {
      servo_service_handler->ServoDumpStatusEvent.SetReceiveHandler([this] () {
        auto res = servo_service_handler->ServoDumpStatusEvent.GetNewSamples();
        if (!res.HasValue()) {
          return;
        }
        this->data.SetServoDumpStatus(res.Value());
      });
    });
    servo_service_handler->ServoVentStatusEvent.Subscribe(1, [this](const uint8_t status){
      servo_service_handler->ServoVentStatusEvent.SetReceiveHandler([this] () {
        auto res = servo_service_handler->ServoVentStatusEvent.GetNewSamples();
        if (!res.HasValue()) {
          return;
        }
        this->data.SetServoVentStatus(res.Value());
      });
    });
  });
  this->primer_service_proxy.StartFindService([this](auto handler) {
    this->primer_service_handler = handler;
    primer_service_handler->primeStatusEvent.Subscribe(1, [this](const uint8_t status) {
      primer_service_handler->primeStatusEvent.SetReceiveHandler([this] () {
        auto res = primer_service_handler->primeStatusEvent.GetNewSamples();
        if (!res.HasValue()) {
          return;
        }
        this->data.SetPrimerStatus(res.Value());
      });
    });
  });
  this->stat_service_proxy.StartFindService([this](auto handler) {
    this->stat_service_handler = handler;
    stat_service_handler->NewSystemUsage.Subscribe(1, [this](const uint8_t status) {
      stat_service_handler->NewSystemUsage.SetReceiveHandler([this] () {
        auto res = stat_service_handler->NewSystemUsage.GetNewSamples();
        if (!res.HasValue()) {
          return;
        }
        this->data.SetSysStatus(res.Value());
      });
    });
  });
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
    env_service_handler->newTensoEvent.Subscribe(1, [this](const uint8_t status) {
      env_service_handler->newTensoEvent.SetReceiveHandler([this] () {
        auto res = env_service_handler->newTensoEvent.GetNewSamples();
        if (!res.HasValue()) {
          return;
        }
        this->data.SetTenso(res.Value());
      });
    });
  });
}

}  // namespace logger
}  // namespace srp
