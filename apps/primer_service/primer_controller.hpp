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
#ifndef APPS_PRIMER_SERVICE_PRIMER_CONTROLLER_HPP_
#define APPS_PRIMER_SERVICE_PRIMER_CONTROLLER_HPP_

#include <string>
#include <vector>
#include <memory>

#include "mw/gpio_server/controller/gpio_controller.hpp"
#include "mw/gpio_server/controller/gpio_controller.hpp"
#include "ara/log/log.h"
#include "core/common/condition.h"
#include "core/json/json_parser.h"

namespace simba {
namespace primer {

namespace {
  /**
   * @brief domyślne wartości
   */
  const constexpr uint8_t IGNITER_PIN_ID = 5;
  const constexpr uint16_t IGNITER_ACTIVE_TIME = 250;
  const constexpr uint8_t ON_INGITER = 1;
  const constexpr uint8_t OFF_INGITER = 0;
}  // namespace

class PrimerController {
 private:
  std::unique_ptr<gpio::GPIOController> gpio_;
  std::vector<uint8_t> primer_pins_;
  std::uint16_t active_time;
  std::uint8_t primerState;

 public:
  explicit PrimerController(std::unique_ptr<gpio::GPIOController> gpio): gpio_(std::move(gpio)) {
  }
  void Initialize(std::string path) {
    this->ReadConfig(path);
    ChangePrimerState(0);
  }

  bool ChangePrimerState(uint8_t state) {
    if (this->primerState != state) {
        for (const auto primer : primer_pins_) {
            this->gpio_->SetPinValue(primer, state);
        }
        this->primerState = state;
        return true;
    }
    return false;
  }
  uint8_t GetPrimerState() {
    return primerState;
  }

 private:
  bool ReadConfig(std::string path) {
    auto parser_opt = core::json::JsonParser::Parser(path);
    this->active_time = IGNITER_ACTIVE_TIME;

    if (!parser_opt.has_value()) {
        ara::log::LogWarn() << "cant find config file, use DEFAULT IGNITER_ID and ACTIVE_TIME";
        this->primer_pins_.push_back(IGNITER_PIN_ID);
        return core::ErrorCode::kInitializeError;
    }
    auto pin_ids_opt = parser_opt.value().GetArray<uint8_t>("primer_ids");
    if (!pin_ids_opt.has_value()) {
        ara::log::LogWarn() << "config dont contains primer field";
    } else {
    this->primer_pins_ = pin_ids_opt.value();
    }
    auto active_time_opt = parser_opt.value().GetNumber<uint16_t>("primer_active_time");
    if (active_time_opt.has_value()) {
        this->active_time = active_time_opt.value();
    }
    return true;
  }
};

}
}

#endif  // APPS_PRIMER_SERVICE_PRIMER_CONTROLLER_HPP_
