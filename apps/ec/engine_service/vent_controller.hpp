/**
 * @file vent_controller.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2026-04-17
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#ifndef APPS_EC_ENGINE_SERVICE_VENT_CONTROLLER_HPP_
#define APPS_EC_ENGINE_SERVICE_VENT_CONTROLLER_HPP_
#include <memory>
#include <functional>
#include <thread>  // NOLINT
#include <mutex>  // NOLINT
namespace srp {
namespace apps {
namespace engine {
using VentMoveFunc = std::function<void(const uint8_t&)>;
using timepoint = std::chrono::_V2::high_resolution_clock::time_point;
enum VentState_e {
    CLOSE = 0,
    OPEN = 1,
    OPENING = 2,
};
class VentController {
 private:
  VentMoveFunc vent_handler;
  VentState_e actual_state;
  std::unique_ptr<std::jthread> opening_thread;
  void VentingLoop(const std::stop_token& t);
  timepoint last_requested_opening;
  std::unique_ptr<std::jthread> auto_close_thread;
  std::mutex mtx_;
 public:
  static std::shared_ptr<VentController> GetInstance() {
        static std::shared_ptr<VentController> instance = std::make_shared<VentController>();
        return instance;
  }
  void BindVentHandler(VentMoveFunc func) { vent_handler = func; }
  void ChangeState(const VentState_e new_state);
  VentController();
};
}  // namespace engine
}  // namespace apps
}  // namespace srp

#endif  // APPS_EC_ENGINE_SERVICE_VENT_CONTROLLER_HPP_
