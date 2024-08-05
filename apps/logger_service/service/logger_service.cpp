/**
 * @file logger_service.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @author Michał Mańkowski (m.mankowski2004@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-06-05
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "apps/logger_service/service/logger_service.hpp"

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
  this->SleepMainThread();
  return core::ErrorCode::kOk;
}

void LoggerService::SaveFunc(std::stop_token stopToken) {
  const float T = 1.1;
  auto lastActuatorSentTime = std::chrono::high_resolution_clock::now();
  auto lastSensorSentTime = std::chrono::high_resolution_clock::now();
  CSVDriver csv_act("/log/actuators");
  csv_act.Init("TIMESTAMP;MAIN_VALVE;VENT_VALVE;PRIMER");
  CSVDriver csv_sens("/log/sensors");
  csv_sens.Init("TIMESTAMP;NOZLE_PRESS;TANK_PRESS;TANK_TEMP_1;TANK_TEMP_2;TANK_TEMP_3;TANK_D_PRESS");
  while (!stopToken.stop_requested()) {
    bool actuatorConditionsMet = this->gotPrimer && this->gotMainValve && this->gotVentValve;
    bool sensorConditionsMet = this->gotTemp1 && this->gotTemp2 && this->gotTemp3 &&
     this->gotDPress && this->gotTankPress && this->gotNozlePress;
    auto currentTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsedActuator = currentTime - lastActuatorSentTime;
    std::chrono::duration<double> elapsedSensor = currentTime - lastSensorSentTime;
    if (actuatorConditionsMet || elapsedActuator.count() >= T) {
      csv_act.WriteLine(data_actuator_, 1);
      lastActuatorSentTime = std::chrono::high_resolution_clock::now();
      this->gotPrimer = false, this->gotMainValve = false, this->gotVentValve = false;
    }
    if (sensorConditionsMet || elapsedSensor.count() >= T) {
      csv_sens.WriteLine(data_, 1);
      lastSensorSentTime = std::chrono::high_resolution_clock::now();
      this->gotTemp1 = false, this->gotTemp2 = false, this->gotTemp3 = false, this->gotDPress = false,
       this->gotTankPress = false, this->gotNozlePress = false;
    }
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

core::ErrorCode LoggerService::Initialize(
    const std::unordered_map<std::string, std::string>& parms) {
  this->primer_event =
      std::make_shared<com::someip::EventProxyBase>(PRIM_EVENT,
                                            [this](const std::vector<uint8_t> payload) {
      if (payload.size() != 1) {
        return;
      }
      std::lock_guard<std::mutex> lock(actuator_data_mtx);
      this->data_actuator_.primer = payload[0];
      this->gotPrimer = true;
    });
  this->d_press_event =
      std::make_shared<com::someip::EventProxyBase>(D_PRESS_EVENT,
                                            [this](const std::vector<uint8_t> payload) {
      if (payload.size() != 2) {
        return;
      }
      std::lock_guard<std::mutex> lock(data_mtx);
      this->data_.d_press = convertTemp(payload);
      this->gotDPress = true;
    });
  this->tank_press_event =
      std::make_shared<com::someip::EventProxyBase>(TANK_PRESS_EVENT,
                                            [this](const std::vector<uint8_t> payload) {
      if (payload.size() != 2) {
        return;
      }
      std::lock_guard<std::mutex> lock(data_mtx);
      this->data_.tank_press = convertTemp(payload);
      this->gotTankPress = true;
      return;
    });
  this->nozle_press_event =
      std::make_shared<com::someip::EventProxyBase>(NOZLE_PRESS_EVENT,
                                            [this](const std::vector<uint8_t> payload) {
      if (payload.size() != 2) {
        return;
      }
      std::lock_guard<std::mutex> lock(data_mtx);
      this->data_.nozle_press = convertTemp(payload);
      this->gotNozlePress = true;
    });
  this->temp_1_event =
      std::make_shared<com::someip::EventProxyBase>(TANK_TEMP1_EVENT,
                                            [this](const std::vector<uint8_t> payload) {
      if (payload.size() != 2) {
        return;
      }
      AppLogger::Info("Receive " + std::to_string(convertTemp(payload)));
      std::lock_guard<std::mutex> lock(data_mtx);
      this->data_.temp_1 = convertTemp(payload);
      this->gotTemp1 = true;
    });
  this->temp_2_event =
      std::make_shared<com::someip::EventProxyBase>(TANK_TEMP2_EVENT,
                                            [this](const std::vector<uint8_t> payload) {
      if (payload.size() != 2) {
        return;
      }
      AppLogger::Info("Receive " + std::to_string(convertTemp(payload)));
      std::lock_guard<std::mutex> lock(data_mtx);
      this->data_.temp_2 = convertTemp(payload);
      this->gotTemp2 = true;
    });
  this->temp_3_event =
      std::make_shared<com::someip::EventProxyBase>(TANK_TEMP3_EVENT,
                                            [this](const std::vector<uint8_t> payload) {
      if (payload.size() != 2) {
        return;
      }
      AppLogger::Info("Receive " + std::to_string(convertTemp(payload)));
      std::lock_guard<std::mutex> lock(data_mtx);
      this->data_.temp_3 = convertTemp(payload);
      this->gotTemp3 = true;
    });
  this->main_valve_event =
      std::make_shared<com::someip::EventProxyBase>(MAIN_VALVE_EVENT,
                                            [this](const std::vector<uint8_t> payload) {
      if (payload.size() != 1) {
        return;
      }
      std::lock_guard<std::mutex> lock(actuator_data_mtx);
      this->data_actuator_.main_valve = payload[0];
      this->gotMainValve = true;
    });
  this->vent_valve_event =
      std::make_shared<com::someip::EventProxyBase>(VENT_VALVE_EVENT,
                                            [this](const std::vector<uint8_t> payload) {
      if (payload.size() != 1) {
        return;
      }
      std::lock_guard<std::mutex> lock(actuator_data_mtx);
      this->data_actuator_.vent_valve = payload[0];
      this->gotVentValve = true;
    });

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
