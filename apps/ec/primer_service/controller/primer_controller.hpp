/**
 * @file primer_controller.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-10-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef APPS_EC_PRIMER_SERVICE_CONTROLLER_PRIMER_CONTROLLER_HPP_
#define APPS_EC_PRIMER_SERVICE_CONTROLLER_PRIMER_CONTROLLER_HPP_

#include <string>
#include <vector>
#include <utility>
#include <memory>

#include "mw/gpio_server/controller/gpio_controller.hpp"
#include "ara/log/log.h"
#include "ara/log/logging_menager.h"
#include "core/common/condition.h"
#include "core/json/json_parser.h"
#include "mw/i2c_service/controller/ads7828/controller.hpp"

namespace srp {
namespace primer {

enum PrimerState_t {
  kUNKNOWN,
  kNOT_CONNECTED,
  kCONNECTED,
  kSHORT_CIRCUIT,
  kFIRED,
};

class PrimerController {
 private:
  const ara::log::Logger& prim_logger;
  gpio::GPIOController gpio_;
  std::atomic<PrimerState_t> primerState;
  i2c::ADS7828 adc_;

 public:
  PrimerController();
  void Initialize();

  bool EnablePrimer(const bool auto_disable = true);
  bool DisablePrimer();
  PrimerState_t GetPrimerState() const noexcept;
  void VerifyPrimerConection();

};

}  // namespace primer
}  // namespace srp

#endif  // APPS_EC_PRIMER_SERVICE_CONTROLLER_PRIMER_CONTROLLER_HPP_
