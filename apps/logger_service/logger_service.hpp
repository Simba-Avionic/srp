/**
 * @file logger_service.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-06-05
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef APPS_LOGGER_SERVICE_LOGGER_SERVICE_HPP_
#define APPS_LOGGER_SERVICE_LOGGER_SERVICE_HPP_

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <thread>  // NOLINT
#include <chrono>  // NOLINT


#include "core/application/application_no_ipc.h"
#include "communication-core/someip-controller/event_proxy.h"
#include "communication-core/someip-controller/method_skeleton.h"
#include "diag/dtc/controller/dtc.h"
namespace simba {
namespace logger {

enum MSG_TYPE {
  MAIN_VALVE,
  VENT_VALVE,
  PRIMER,
  TANK_PRESS,
  NOZLE_PRESS,
  TANK_D_PRESS,
  TANK_TEMP1,
  TANK_TEMP2,
  TANK_TEMP3
};

class ActuatorData {
 public:
  uint8_t primer;
  uint8_t main_valve;
  uint8_t vent_valve;
};

class SensorData {
 public:
  float temp_1;
  float temp_2;
  float temp_3;
  float d_press;
  float tank_press;
  float nozle_press;
};

class LoggerService final : public core::ApplicationNoIPC {
 protected:
  void SaveFunc(std::stop_token stopToken);
  core::ErrorCode StopThread();
  com::someip::EventCallback CallbackGenerator(MSG_TYPE type);
  /**
   * @brief This function is called to launch the application
   *
   * @param token stop token
   */
  core::ErrorCode Run(std::stop_token token) final;
  /**
   * @brief This function is called to initialize the application
   *
   * @param parms map with parms
   */
  core::ErrorCode Initialize(
      const std::unordered_map<std::string, std::string>& parms) final;

  std::mutex data_mtx;
  SensorData data_;

  std::mutex actuator_data_mtx;
  ActuatorData data_actuator_;

  // events
  std::shared_ptr<simba::com::someip::EventProxyBase> primer_event;
  std::shared_ptr<simba::com::someip::EventProxyBase> main_valve_event;
  std::shared_ptr<simba::com::someip::EventProxyBase> vent_valve_event;
  std::shared_ptr<simba::com::someip::EventProxyBase> temp_1_event;
  std::shared_ptr<simba::com::someip::EventProxyBase> temp_2_event;
  std::shared_ptr<simba::com::someip::EventProxyBase> temp_3_event;
  std::shared_ptr<simba::com::someip::EventProxyBase> d_press_event;
  std::shared_ptr<simba::com::someip::EventProxyBase> tank_press_event;
  std::shared_ptr<simba::com::someip::EventProxyBase> nozle_press_event;

  std::shared_ptr<simba::com::someip::MethodSkeleton> start_method;
  std::shared_ptr<simba::com::someip::MethodSkeleton> stop_method;

  std::unique_ptr<std::jthread> save_thread_;
};

}  // namespace logger
}  // namespace simba
#endif  // APPS_LOGGER_SERVICE_LOGGER_SERVICE_HPP_
