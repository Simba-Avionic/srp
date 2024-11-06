/**
 * @file primer_controller.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-10-30
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#include <string>
#include <vector>
#include <utility>
#include <memory>

#include "mw/gpio_server/controller/gpio_controller.hpp"
#include "ara/log/log.h"
#include "core/common/condition.h"
#include "core/json/json_parser.h"
#include "apps/primer_service/controller/primer_controller.hpp"

namespace simba {
namespace primer {

namespace {
  /**
   * @brief domyślne wartości
   */
  const constexpr uint8_t kIgniter_pin_id = 5;
  const constexpr uint16_t kIgniter_active_time = 250;
  const constexpr uint8_t kOff_ignite = 0;
}  // namespace

PrimerController::PrimerController(std::unique_ptr<gpio::GPIOController> gpio): gpio_(std::move(gpio)) {
}
void PrimerController::Initialize(std::string path) {
    this->ReadConfig(path);
    ChangePrimerState(kOff_ignite);
}

bool PrimerController::ChangePrimerState(uint8_t state) {
    if (this->primerState != state) {
        for (const auto primer : primer_pins_) {
            this->gpio_->SetPinValue(primer, state);
        }
        this->primerState = state;
        return true;
    }
    return false;
}

uint8_t PrimerController::GetPrimerState() {
    return primerState;
}

bool PrimerController::ReadConfig(std::string path) {
    auto parser_opt = core::json::JsonParser::Parser(path);
    this->active_time = kIgniter_active_time;

    if (!parser_opt.has_value()) {
        ara::log::LogWarn() << "cant find config file, use DEFAULT IGNITER_ID and ACTIVE_TIME";
        this->primer_pins_.push_back(kIgniter_pin_id);
        return false;
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

}  // namespace primer
}  // namespace simba

