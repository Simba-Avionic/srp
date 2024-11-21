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
#ifndef APPS_LOGGER_SERVICE_SERVICE_LOGGER_SERVICE_HPP_
#define APPS_LOGGER_SERVICE_SERVICE_LOGGER_SERVICE_HPP_

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <thread>  // NOLINT
#include <chrono>  // NOLINT


#include "core/application/application_no_ipc.h"
#include "communication-core/someip-controller/method_skeleton.h"
#include "diag/dtc/controller/dtc.h"
#include "communication-core/someip-controller/event_proxy.h"
#include "core/common/wait_queue.h"
#include "apps/logger_service/csvdriver/csvdriver.h"
#include "core/json/json_parser.h"
namespace simba {
namespace logger {

class LoggerService final : public core::ApplicationNoIPC {
 protected:
  void SaveFunc(std::stop_token stopToken);
  core::ErrorCode StopThread();
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
  core::ErrorCode ReadConfig(core::json::JsonParser parser);

  core::ErrorCode Initialize(
      const std::unordered_map<std::string, std::string>& parms) final;

  std::string getCurrentTime();

  std::chrono::time_point<std::chrono::steady_clock> start_time{};

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

  bool T = 0;
  bool main_valve = 0;
  bool vent_valve = 0;
  bool primer = 0;
  bool nozle_press = 0;
  bool tank_press = 0;
  bool temp_1 = 0;
  bool temp_2 = 0;
  bool temp_3 = 0;
  bool d_press = 0;

  std::unique_ptr<std::jthread> save_thread_;

  bool gotPrimer = false;
  bool gotMainValve = false;
  bool gotVentValve = false;
  bool gotTemp1 = false;
  bool gotTemp2 = false;
  bool gotTemp3 = false;
  bool gotDPress = false;
  bool gotTankPress = false;
  bool gotNozlePress = false;
};

}  // namespace logger
}  // namespace simba
#endif  // APPS_LOGGER_SERVICE_SERVICE_LOGGER_SERVICE_HPP_
