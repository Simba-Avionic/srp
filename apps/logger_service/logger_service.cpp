/**
 * @file logger_service.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-06-05
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "apps/logger_service/logger_service.hpp"
#include "communication-core/someip-controller/event_proxy.h"

namespace simba {
namespace logger {

namespace {
  constexpr auto PRIM_EVENT = "LoggerApp/primer_status_event";
  constexpr auto D_PRESS_EVENT = "LoggerApp/d_press_event";
  constexpr auto TANK_PRESS_EVENT = "LoggerApp/tank_press_event";
  constexpr auto NOZLE_PRESS_EVENT = "LoggerApp/nozle_press_event";
  constexpr auto TANK_TEMP1_EVENT = "LoggerApp/Temp_Event_1";
  constexpr auto TANK_TEMP2_EVENT = "LoggerApp/Temp_Event_2";
  constexpr auto TANK_TEMP3_EVENT = "LoggerApp/Temp_Event_3";
  constexpr auto MAIN_VALVE_EVENT = "LoggerApp/main_valve";
  constexpr auto VENT_VALVE_EVENT = "LoggerApp/vent_valve";
}  // namespace

float convertTemp(const std::vector<uint8_t> &bytes) {
    if (bytes.size() != sizeof(int16_t)) {
        throw std::invalid_argument("Invalid byte vector size");
    }
    int16_t intValue;
    std::memcpy(&intValue, bytes.data(), sizeof(int16_t));
    return static_cast<float>(intValue) / 10.0;
}

core::ErrorCode LoggerService::Run(std::stop_token token) {
  while (true) {
    this->SleepMainThread();
  }
  return core::ErrorCode::kOk;
}

void LoggerService::SaveFunc(std::stop_token stopToken) {
  while (!stopToken.stop_requested()) {
    // TODO saving process   // NOLINT
  }
}
core::ErrorCode LoggerService::StopThread() {
  if (!save_thread_ && save_thread_->joinable()) {
    save_thread_->request_stop();
    save_thread_->join();
    save_thread_.reset();
    return core::ErrorCode::kOk;
  }
  return core::ErrorCode::kError;
}


com::someip::EventCallback LoggerService::CallbackGenerator(MSG_TYPE type) {
  switch (type) {
  case TANK_PRESS:
    return [this](const std::vector<uint8_t> payload) {
      if (payload.size() != 2) {
        return;
      }
      std::lock_guard<std::mutex> lock(data_mtx);
      this->data_.tank_press = convertTemp(payload);
      return;
    };
    break;
    case NOZLE_PRESS:
    return [this](const std::vector<uint8_t> payload) {
      if (payload.size() != 2) {
        return;
      }
      std::lock_guard<std::mutex> lock(data_mtx);
      this->data_.nozle_press = convertTemp(payload);
    };
    break;
    case TANK_D_PRESS:
    return [this](const std::vector<uint8_t> payload) {
      if (payload.size() != 2) {
        return;
      }
      std::lock_guard<std::mutex> lock(data_mtx);
      this->data_.d_press = convertTemp(payload);
    };
    break;
    case TANK_TEMP1:
    return [this](const std::vector<uint8_t> payload) {
      if (payload.size() != 2) {
        return;
      }
      AppLogger::Info("Receive " + std::to_string(convertTemp(payload)));
      std::lock_guard<std::mutex> lock(data_mtx);
      this->data_.temp_1 = convertTemp(payload);
    };
    break;
    case TANK_TEMP2:
    return [this](const std::vector<uint8_t> payload) {
      if (payload.size() != 2) {
        return;
      }
      AppLogger::Info("Receive " + std::to_string(convertTemp(payload)));
      std::lock_guard<std::mutex> lock(data_mtx);
      this->data_.temp_2 = convertTemp(payload);
    };
    break;
    case TANK_TEMP3:
    return [this](const std::vector<uint8_t> payload) {
      if (payload.size() != 2) {
        return;
      }
      AppLogger::Info("Receive " + std::to_string(convertTemp(payload)));
      std::lock_guard<std::mutex> lock(data_mtx);
      this->data_.temp_3 = convertTemp(payload);
    };
    break;
    case MAIN_VALVE:
    return [this](const std::vector<uint8_t> payload) {
      if (payload.size() != 1) {
        return;
      }
      std::lock_guard<std::mutex> lock(actuator_data_mtx);
      this->data_actuator_.main_valve = payload[0];
    };
    break;
    case VENT_VALVE:
    return [this](const std::vector<uint8_t> payload) {
      if (payload.size() != 1) {
        return;
      }
      std::lock_guard<std::mutex> lock(actuator_data_mtx);
      this->data_actuator_.vent_valve = payload[0];
    };
    break;
    case PRIMER:
    return [this](const std::vector<uint8_t> payload) {
      if (payload.size() != 1) {
        return;
      }
      std::lock_guard<std::mutex> lock(actuator_data_mtx);
      this->data_actuator_.primer = payload[0];
    };
    break;
  default:
    return [](const std::vector<uint8_t> payload) {
      AppLogger::Warning("Invalid data type");
      return;
    };
    break;
  }
}

core::ErrorCode LoggerService::Initialize(
    const std::unordered_map<std::string, std::string>& parms) {
  this->primer_event =
      std::make_shared<com::someip::EventProxyBase>(PRIM_EVENT,
                                            CallbackGenerator(MSG_TYPE::PRIMER));
  this->d_press_event =
      std::make_shared<com::someip::EventProxyBase>(D_PRESS_EVENT,
                                            CallbackGenerator(MSG_TYPE::TANK_D_PRESS));
  this->tank_press_event =
      std::make_shared<com::someip::EventProxyBase>(TANK_PRESS_EVENT,
                                            CallbackGenerator(MSG_TYPE::TANK_PRESS));
  this->nozle_press_event =
      std::make_shared<com::someip::EventProxyBase>(NOZLE_PRESS_EVENT,
                                            CallbackGenerator(MSG_TYPE::NOZLE_PRESS));
  this->temp_1_event =
      std::make_shared<com::someip::EventProxyBase>(TANK_TEMP1_EVENT,
                                            CallbackGenerator(MSG_TYPE::TANK_TEMP1));
  this->temp_2_event =
      std::make_shared<com::someip::EventProxyBase>(TANK_TEMP2_EVENT,
                                            CallbackGenerator(MSG_TYPE::TANK_TEMP2));
  this->temp_3_event =
      std::make_shared<com::someip::EventProxyBase>(TANK_TEMP3_EVENT,
                                            CallbackGenerator(MSG_TYPE::TANK_TEMP3));
  this->main_valve_event =
      std::make_shared<com::someip::EventProxyBase>(MAIN_VALVE_EVENT,
                                            CallbackGenerator(MSG_TYPE::MAIN_VALVE));
  this->vent_valve_event =
      std::make_shared<com::someip::EventProxyBase>(VENT_VALVE_EVENT,
                                            CallbackGenerator(MSG_TYPE::VENT_VALVE));

  this->start_method = std::make_shared<com::someip::MethodSkeleton>(
    "LoggerApp/start",
    [this](const std::vector<uint8_t> payload)
        -> std::optional<std::vector<uint8_t>> {
  if (!save_thread_) {
    this->save_thread_ = std::make_unique<std::jthread>([this](std::stop_token st) { this->SaveFunc(st); });
      return std::vector<uint8_t>{1};
  }
  return std::vector<uint8_t>{0};
  });
  this->stop_method = std::make_shared<com::someip::MethodSkeleton>(
      "LoggerApp/stop",
      [this](const std::vector<uint8_t> payload)
          -> std::optional<std::vector<uint8_t>> {
    if (this->StopThread() == core::ErrorCode::kOk) {
      return std::vector<uint8_t>{1};
    }
    return std::vector<uint8_t>{0};
  });
  /**
   * @brief register SOME/IP events and methods in network
   * 
   */
  com->Add(primer_event);
  com->Add(d_press_event);
  com->Add(tank_press_event);
  com->Add(nozle_press_event);
  com->Add(temp_1_event);
  com->Add(temp_2_event);
  com->Add(temp_3_event);
  com->Add(main_valve_event);
  com->Add(vent_valve_event);
  com->Add(start_method);
  com->Add(stop_method);

  /**
   * @brief find event
   * 
   */
  temp_1_event->StartFindService();
  temp_2_event->StartFindService();
  temp_3_event->StartFindService();
  main_valve_event->StartFindService();
  vent_valve_event->StartFindService();
  tank_press_event->StartFindService();
  nozle_press_event->StartFindService();

  return core::ErrorCode::kOk;
}

}  // namespace logger
}  // namespace simba
