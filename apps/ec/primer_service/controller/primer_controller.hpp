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
  gpio::GPIOController gpio_;
  std::vector<uint8_t> primer_pins_;
  std::uint16_t active_time;
  std::atomic<PrimerState_t> primerState;
  i2c::ADS7828 adc_;

 public:
  PrimerController();
  void Initialize(std::string path);

  bool EnablePrimer(const bool auto_disable = true);
  bool DisablePrimer();
  PrimerState_t GetPrimerState() const noexcept;
  void VerifyPrimerConection();

 private:
  bool ReadConfig(std::string path);
};

}  // namespace primer
}  // namespace srp

#endif  // APPS_EC_PRIMER_SERVICE_CONTROLLER_PRIMER_CONTROLLER_HPP_
