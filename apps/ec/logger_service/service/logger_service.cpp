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
  static constexpr uint16_t kSave_interval = 10;
  static constexpr auto kEnv_service_path_name = "srp/apps/FileLoggerApp/EnvApp";
  static constexpr auto kUdp_service_path_name = "srp/apps/FileLoggerApp/logService_udp";
  static constexpr auto kIpc_service_path_name = "srp/apps/FileLoggerApp/logService_ipc";
  static constexpr auto kSysStat_service_path_name = "srp/apps/FileLoggerApp/SysStatService";
  static constexpr auto kPrimer_service_path_name = "srp/apps/FileLoggerApp/PrimerService";
  static constexpr auto kServo_service_path_name = "srp/apps/FileLoggerApp/ServoService";
  static constexpr auto kEngine_service_path_name = "srp/apps/FileLoggerApp/EngineService";
  static constexpr auto kSecEnv_service_path_name = "srp/apps/FileLoggerApp/SecEnvApp";
  static constexpr auto kSecServo_service_path_name = "srp/apps/FileLoggerApp/SecServoService";
  static constexpr auto kFile_did_path_name = "/srp/apps/FileLoggerApp/logger_did";
  static constexpr auto kLogs_on = 1;
  static constexpr auto kLogs_off = 0;
  static constexpr auto kHeartBeatPinID = 2;
  static constexpr auto gpios_sub = {1, 2, 3, 4, 5, 6, 7, 8, 9,
                10, 11, 12, 13, 14, 15, 16, 17};
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
  start_func_handler(1);
  while (!token.stop_requested()) {
    if (gpio_.SetPinValue(kHeartBeatPinID, 1, 500) != core::ErrorCode::kOk) {
      ara::log::LogWarn() << "EngineApp::Run: Failed to toggle heartbeat pin";
    }
    service_ipc->LoggingState.Update(save_state.load());
    service_udp->LoggingState.Update(save_state.load());
    core::condition::wait_for(std::chrono::milliseconds(1000), token);
  }
  start_func_handler(0);
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
  gpio_.SetCallback([this](uint8_t pin_id, uint8_t state) {
    data.SetGpioState(pin_id, state);
  });
  for (const auto& id : gpios_sub) {
    if (gpio_.ManagePinSubscription(id, true) != core::ErrorCode::kOk) {
      ara::log::LogError() << "Failed to subscribe pin id: " << static_cast<int>(id);
    }
  }
  return 0;
}

LoggerService::~LoggerService() {}

LoggerService::LoggerService():
      someip_logger{ara::log::LoggingMenager::GetInstance()->CreateLogger(
          "SOME", "", ara::log::LogLevel::kWarn)},
      env_service_proxy{ara::core::InstanceSpecifier{kEnv_service_path_name}},
      stat_service_proxy{ara::core::InstanceSpecifier{kSysStat_service_path_name}},
      primer_service_proxy{ara::core::InstanceSpecifier{kPrimer_service_path_name}},
      servo_service_proxy{ara::core::InstanceSpecifier{kServo_service_path_name}},
      engine_service_proxy{ara::core::InstanceSpecifier{kEngine_service_path_name}},
      sec_env_service_proxy{ara::core::InstanceSpecifier{kSecEnv_service_path_name}},
      sec_servo_service_proxy{ara::core::InstanceSpecifier{kSecServo_service_path_name}},
      env_service_handler{nullptr},
      stat_service_handler{nullptr},
      primer_service_handler{nullptr},
      servo_service_handler{nullptr},
      engine_service_handler{nullptr},
      sec_env_service_handler{nullptr},
      sec_servo_service_handler{nullptr},
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
  ara::log::LogWarn() << "LoggerService::start_func_handler: status=" << status
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
                               << status;
      engine_service_handler->CurrentMode.SetReceiveHandler([this] () {
        auto res = engine_service_handler->CurrentMode.GetNewSamples();
        if (!res.HasValue()) {
          return;
        }
        someip_logger.LogDebug() << "Engine CurrentMode sample: "
                                 << res.Value();
        this->data.SetEngineMode(res.Value());
      });
    });
  });
  this->servo_service_proxy.StartFindService([this](auto handler) {
    someip_logger.LogDebug() << "Servo service handler discovered";
    this->servo_service_handler = handler;
    servo_service_handler->newOxidizerMainValveEvent.Subscribe(1, [this](const uint8_t status) {
      someip_logger.LogDebug() << "Subscribed to newOxidizerMainValveEvent, status="
                               << status;
      servo_service_handler->newOxidizerMainValveEvent.SetReceiveHandler([this] () {
        auto res = servo_service_handler->newOxidizerMainValveEvent.GetNewSamples();
        if (!res.HasValue()) {
          return;
        }
        someip_logger.LogDebug() << "newOxidizerMainValveEvent sample: "
                                 << res.Value();
        this->data.SetOxidizerMainValve(res.Value());
      });
    });
    servo_service_handler->newOxidizerVentValveEvent.Subscribe(1, [this](const uint8_t status) {
      someip_logger.LogDebug() << "Subscribed to newOxidizerVentValveEvent, status="
                               << status;
      servo_service_handler->newOxidizerVentValveEvent.SetReceiveHandler([this] () {
        auto res = servo_service_handler->newOxidizerVentValveEvent.GetNewSamples();
        if (!res.HasValue()) {
          return;
        }
        someip_logger.LogDebug() << "newOxidizerVentValveEvent sample: "
                                 << res.Value();
        this->data.SetOxidizerVentValve(res.Value());
      });
    });
    servo_service_handler->newOxidizerDumpValveEvent.Subscribe(1, [this](const uint8_t status) {
      someip_logger.LogDebug() << "Subscribed to newOxidizerDumpValveEvent, status="
                               << status;
      servo_service_handler->newOxidizerDumpValveEvent.SetReceiveHandler([this] () {
        auto res = servo_service_handler->newOxidizerDumpValveEvent.GetNewSamples();
        if (!res.HasValue()) {
          return;
        }
        someip_logger.LogDebug() << "newOxidizerDumpValveEvent sample: "
                                 << res.Value();
        this->data.SetOxidizerDumpValve(res.Value());
      });
    });
    servo_service_handler->newPressureFeedMainValveEvent.Subscribe(1, [this](const uint8_t status) {
      someip_logger.LogDebug() << "Subscribed to newPressureFeedMainValveEvent, status="
                               << status;
      servo_service_handler->newPressureFeedMainValveEvent.SetReceiveHandler([this] () {
        auto res = servo_service_handler->newPressureFeedMainValveEvent.GetNewSamples();
        if (!res.HasValue()) {
          return;
        }
        someip_logger.LogDebug() << "newPressureFeedMainValveEvent sample: "
                                 << res.Value();
        this->data.SetPressureFeedMainValve(res.Value());
      });
    });
    servo_service_handler->newPressureFeedVentValveEvent.Subscribe(1, [this](const uint8_t status) {
      someip_logger.LogDebug() << "Subscribed to newPressureFeedVentValveEvent, status="
                               << status;
      servo_service_handler->newPressureFeedVentValveEvent.SetReceiveHandler([this] () {
        auto res = servo_service_handler->newPressureFeedVentValveEvent.GetNewSamples();
        if (!res.HasValue()) {
          return;
        }
        someip_logger.LogDebug() << "newPressureFeedVentValveEvent sample: "
                                 << res.Value();
        this->data.SetPressureFeedVentValve(res.Value());
      });
    });
  });
  this->primer_service_proxy.StartFindService([this](auto handler) {
    someip_logger.LogDebug() << "Primer service handler discovered";
    this->primer_service_handler = handler;
    primer_service_handler->primeStatusEvent.Subscribe(1, [this](const uint8_t status) {
      someip_logger.LogDebug() << "Subscribed to Primer primeStatusEvent, status="
                               << status;
      primer_service_handler->primeStatusEvent.SetReceiveHandler([this] () {
        auto res = primer_service_handler->primeStatusEvent.GetNewSamples();
        if (!res.HasValue()) {
          return;
        }
        someip_logger.LogDebug() << "Primer primeStatusEvent sample: "
                                 << res.Value();
        this->data.SetPrimerStatus(res.Value());
      });
    });
  });
  this->stat_service_proxy.StartFindService([this](auto handler) {
    someip_logger.LogDebug() << "SysStat service handler discovered";
    this->stat_service_handler = handler;
    stat_service_handler->NewSystemUsage.Subscribe(1, [this](const uint8_t status) {
      someip_logger.LogDebug() << "Subscribed to SysStat NewSystemUsage, status="
                               << status;
      stat_service_handler->NewSystemUsage.SetReceiveHandler([this] () {
        auto res = stat_service_handler->NewSystemUsage.GetNewSamples();
        if (!res.HasValue()) {
          return;
        }
        someip_logger.LogDebug() << "SysStat NewSystemUsage sample: cpu="
                                 << res.Value().cpu_usage
                                 << ", mem=" << res.Value().mem_usage
                                 << ", disk=" << res.Value().disk_utilization;
        this->data.SetSysStatus(res.Value());
      });
    });
  });
  this->env_service_proxy.StartFindService([this](auto handler) {
    someip_logger.LogDebug() << "Env service handler discovered";
    this->env_service_handler = handler;
    env_service_handler->newTempEvent_1.Subscribe(1, [this](const uint8_t status) {
      someip_logger.LogDebug() << "Subscribed to Env newTempEvent_1, status="
                               << status;
      env_service_handler->newTempEvent_1.SetReceiveHandler([this] () {
        auto res = env_service_handler->newTempEvent_1.GetNewSamples();
        if (!res.HasValue()) {
          return;
        }
        someip_logger.LogDebug() << "Env newTempEvent_1 sample: "
                                 << res.Value();
        this->data.SetTemp1(res.Value());
      });
    });
    env_service_handler->newTempEvent_2.Subscribe(1, [this](const uint8_t status) {
      someip_logger.LogDebug() << "Subscribed to Env newTempEvent_2, status="
                               << status;
      env_service_handler->newTempEvent_2.SetReceiveHandler([this] () {
        auto res = env_service_handler->newTempEvent_2.GetNewSamples();
        if (!res.HasValue()) {
          return;
        }
        someip_logger.LogDebug() << "Env newTempEvent_2 sample: "
                                 << res.Value();
        this->data.SetTemp2(res.Value());
      });
    });
    env_service_handler->newTempEvent_3.Subscribe(1, [this](const uint8_t status) {
      someip_logger.LogDebug() << "Subscribed to Env newTempEvent_3, status="
                               << status;
      env_service_handler->newTempEvent_3.SetReceiveHandler([this] () {
        auto res = env_service_handler->newTempEvent_3.GetNewSamples();
        if (!res.HasValue()) {
          return;
        }
        someip_logger.LogDebug() << "Env newTempEvent_3 sample: "
                                 << res.Value();
        this->data.SetTemp3(res.Value());
      });
    });
    env_service_handler->newBoardTempEvent1.Subscribe(1, [this](const uint8_t status) {
      someip_logger.LogDebug() << "Subscribed to Env newBoardTempEvent1, status="
                               << status;
      env_service_handler->newBoardTempEvent1.SetReceiveHandler([this] () {
        auto res = env_service_handler->newBoardTempEvent1.GetNewSamples();
        if (!res.HasValue()) {
          return;
        }
        someip_logger.LogDebug() << "Env newBoardTempEvent1 sample: "
                                 << res.Value();
        this->data.SetBoardTemp1(res.Value());
      });
    });
    env_service_handler->newBoardTempEvent2.Subscribe(1, [this](const uint8_t status) {
      someip_logger.LogDebug() << "Subscribed to Env newBoardTempEvent2, status="
                               << status;
      env_service_handler->newBoardTempEvent2.SetReceiveHandler([this] () {
        auto res = env_service_handler->newBoardTempEvent2.GetNewSamples();
        if (!res.HasValue()) {
          return;
        }
        someip_logger.LogDebug() << "Env newBoardTempEvent2 sample: "
                                 << res.Value();
        this->data.SetBoardTemp2(res.Value());
      });
    });
    env_service_handler->newBoardTempEvent3.Subscribe(1, [this](const uint8_t status) {
      someip_logger.LogDebug() << "Subscribed to Env newBoardTempEvent3, status="
                               << status;
      env_service_handler->newBoardTempEvent3.SetReceiveHandler([this] () {
        auto res = env_service_handler->newBoardTempEvent3.GetNewSamples();
        if (!res.HasValue()) {
          return;
        }
        someip_logger.LogDebug() << "Env newBoardTempEvent3 sample: "
                                 << res.Value();
        this->data.SetBoardTemp3(res.Value());
      });
    });
    env_service_handler->newOxidizerPressEvent.Subscribe(1, [this](const uint8_t status) {
      someip_logger.LogDebug() << "Subscribed to Env newOxidizerPressEvent, status="
                               << status;
      env_service_handler->newOxidizerPressEvent.SetReceiveHandler([this] () {
        auto res = env_service_handler->newOxidizerPressEvent.GetNewSamples();
        if (!res.HasValue()) {
          return;
        }
        someip_logger.LogDebug() << "Env newOxidizerPressEvent sample: "
                                 << res.Value();
        this->data.SetOxidizerPress(res.Value());
      });
    });
    env_service_handler->newPressureFeedPressEvent.Subscribe(1, [this](const uint8_t status) {
      someip_logger.LogDebug() << "Subscribed to Env newPressureFeedPressEvent, status="
                               << status;
      env_service_handler->newPressureFeedPressEvent.SetReceiveHandler([this] () {
        auto res = env_service_handler->newPressureFeedPressEvent.GetNewSamples();
        if (!res.HasValue()) {
          return;
        }
        someip_logger.LogDebug() << "Env newPressureFeedPressEvent sample: "
                                 << res.Value();
        this->data.SetPressureFeedPress(res.Value());
      });
    });
    env_service_handler->newChamberPressEvent1.Subscribe(1, [this](const uint8_t status) {
      someip_logger.LogDebug() << "Subscribed to Env newChamberPressEvent1, status="
                               << status;
      env_service_handler->newChamberPressEvent1.SetReceiveHandler([this] () {
        auto res = env_service_handler->newChamberPressEvent1.GetNewSamples();
        if (!res.HasValue()) {
          return;
        }
        someip_logger.LogDebug() << "Env newChamberPressEvent1 sample: "
                                 << res.Value();
        this->data.SetChamberPress1(res.Value());
      });
    });
  });
  this->sec_env_service_proxy.StartFindService([this](auto handler) {
    someip_logger.LogDebug() << "SecEnv service handler discovered";
    this->sec_env_service_handler = handler;
    sec_env_service_handler->newEthanolPressEvent.Subscribe(1, [this](const uint8_t status) {
      someip_logger.LogDebug() << "Subscribed to SecEnv newEthanolPressEvent, status="
                               << status;
      sec_env_service_handler->newEthanolPressEvent.SetReceiveHandler([this] () {
        auto res = sec_env_service_handler->newEthanolPressEvent.GetNewSamples();
        if (!res.HasValue()) {
          return;
        }
        someip_logger.LogDebug() << "SecEnv newEthanolPressEvent sample: "
                                 << res.Value();
        this->data.SetEthanolPress(res.Value());
      });
    });
    sec_env_service_handler->newChamberPressEvent2.Subscribe(1, [this](const uint8_t status) {
      someip_logger.LogDebug() << "Subscribed to SecEnv newChamberPressEvent2, status="
                               << status;
      sec_env_service_handler->newChamberPressEvent2.SetReceiveHandler([this] () {
        auto res = sec_env_service_handler->newChamberPressEvent2.GetNewSamples();
        if (!res.HasValue()) {
          return;
        }
        someip_logger.LogDebug() << "SecEnv newChamberPressEvent2 sample: "
                                 << res.Value();
        this->data.SetChamberPress2(res.Value());
      });
    });
    sec_env_service_handler->newChamberPressEvent3.Subscribe(1, [this](const uint8_t status) {
      someip_logger.LogDebug() << "Subscribed to SecEnv newChamberPressEvent3, status="
                               << status;
      sec_env_service_handler->newChamberPressEvent3.SetReceiveHandler([this] () {
        auto res = sec_env_service_handler->newChamberPressEvent3.GetNewSamples();
        if (!res.HasValue()) {
          return;
        }
        someip_logger.LogDebug() << "SecEnv newChamberPressEvent3 sample: "
                                 << res.Value();
        this->data.SetChamberPress3(res.Value());
      });
    });
    sec_env_service_handler->newBoardTempEvent1.Subscribe(1, [this](const uint8_t status) {
      someip_logger.LogDebug() << "Subscribed to SecEnv newBoardTempEvent1, status="
                               << status;
      sec_env_service_handler->newBoardTempEvent1.SetReceiveHandler([this] () {
        auto res = sec_env_service_handler->newBoardTempEvent1.GetNewSamples();
        if (!res.HasValue()) {
          return;
        }
        someip_logger.LogDebug() << "SecEnv newBoardTempEvent1 sample: "
                                 << res.Value();
        this->data.SetSecBoardTemp1(res.Value());
      });
    });
    sec_env_service_handler->newBoardTempEvent2.Subscribe(1, [this](const uint8_t status) {
      someip_logger.LogDebug() << "Subscribed to SecEnv newBoardTempEvent2, status="
                               << status;
      sec_env_service_handler->newBoardTempEvent2.SetReceiveHandler([this] () {
        auto res = sec_env_service_handler->newBoardTempEvent2.GetNewSamples();
        if (!res.HasValue()) {
          return;
        }
        someip_logger.LogDebug() << "SecEnv newBoardTempEvent2 sample: "
                                 << res.Value();
        this->data.SetSecBoardTemp2(res.Value());
      });
    });
    sec_env_service_handler->newBoardTempEvent3.Subscribe(1, [this](const uint8_t status) {
      someip_logger.LogDebug() << "Subscribed to SecEnv newBoardTempEvent3, status="
                               << status;
      sec_env_service_handler->newBoardTempEvent3.SetReceiveHandler([this] () {
        auto res = sec_env_service_handler->newBoardTempEvent3.GetNewSamples();
        if (!res.HasValue()) {
          return;
        }
        someip_logger.LogDebug() << "SecEnv newBoardTempEvent3 sample: "
                                 << res.Value();
        this->data.SetSecBoardTemp3(res.Value());
      });
    });
  });
  this->sec_servo_service_proxy.StartFindService([this](auto handler) {
    someip_logger.LogDebug() << "SecServo service handler discovered";
    this->sec_servo_service_handler = handler;
    sec_servo_service_handler->newEthanolMainValveEvent.Subscribe(1, [this](const uint8_t status) {
      someip_logger.LogDebug() << "Subscribed to newEthanolMainValveEvent, status="
                               << status;
      sec_servo_service_handler->newEthanolMainValveEvent.SetReceiveHandler([this] () {
        auto res = sec_servo_service_handler->newEthanolMainValveEvent.GetNewSamples();
        if (!res.HasValue()) {
          return;
        }
        someip_logger.LogDebug() << "newEthanolMainValveEvent sample: "
                                 << res.Value();
        this->data.SetEthanolMainValve(res.Value());
      });
    });
    sec_servo_service_handler->newEthanolVentValveEvent.Subscribe(1, [this](const uint8_t status) {
      someip_logger.LogDebug() << "Subscribed to newEthanolVentValveEvent, status="
                               << status;
      sec_servo_service_handler->newEthanolVentValveEvent.SetReceiveHandler([this] () {
        auto res = sec_servo_service_handler->newEthanolVentValveEvent.GetNewSamples();
        if (!res.HasValue()) {
          return;
        }
        someip_logger.LogDebug() << "newEthanolVentValveEvent sample: "
                                 << res.Value();
        this->data.SetEthanolVentValve(res.Value());
      });
    });
  });
}

}  // namespace logger
}  // namespace srp
