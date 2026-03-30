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
#include "apps/ec/primer_service/controller/primer_controller.hpp"

namespace srp {
namespace primer {

namespace {
  const constexpr uint8_t kIgniter_pin_id = 13;
  const constexpr uint16_t kIgniter_active_time = 1000;
  const constexpr uint8_t kPrimer_on = 1;
  static constexpr bool kRequirePrimerVeryfication = false;
}  // namespace

PrimerController::PrimerController():
            gpio_(std::make_unique<com::soc::StreamIpcSocket>()) {
}

void PrimerController::VerifyPrimerConection() {
    // PLACEHOLDER FOR CONNECTION VALIDATION
}

void PrimerController::Initialize(std::string path) {
    this->primerState = PrimerState_t::kUNKNOWN;
    this->ReadConfig(path);
    // TODO(matikrajek42@gmail.com) write veryfication of voltage on IGN_MEASURE wchich say that primer is connected
}

bool PrimerController::DisablePrimer() {
    if (primerState != PrimerState_t::kFIRED) {
        return false;
    }
    for (const auto primer : primer_pins_) {
        if (this->gpio_.SetPinValue(primer, !kPrimer_on, 0) != core::ErrorCode::kOk) {
            ara::log::LogError() << "Failed to disable primer";
            return false;
        }
    }
}

bool PrimerController::EnablePrimer(const bool auto_disable) {
    if (kRequirePrimerVeryfication & primerState != PrimerState_t::kCONNECTED) {
        return false;
    }
    if (primerState == PrimerState_t::kFIRED) {
        return false;
    }
    for (const auto primer : primer_pins_) {
        if (this->gpio_.SetPinValue(primer, kPrimer_on, auto_disable ? active_time : 0) != core::ErrorCode::kOk) {
            ara::log::LogError() << "Failed to enable primer";
            return false;
        }
    }
    primerState = PrimerState_t::kFIRED;
    return true;
}
PrimerState_t PrimerController::GetPrimerState() const noexcept {
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

    auto primer_obj_opt = parser_opt.value().GetObject("primer");
    if (!primer_obj_opt.has_value()) {
        ara::log::LogWarn() << "config dont contains primer field";
        this->primer_pins_.push_back(kIgniter_pin_id);
        return false;
    }

    auto pin_ids_opt = primer_obj_opt.value().GetArray<uint8_t>("primer_ids");
    if (!pin_ids_opt.has_value()) {
        ara::log::LogWarn() << "config dont contains primer_ids field";
        this->primer_pins_.push_back(kIgniter_pin_id);
    } else {
        this->primer_pins_ = pin_ids_opt.value();
    }

    auto active_time_opt = primer_obj_opt.value().GetNumber<uint16_t>("active_time");
    if (active_time_opt.has_value()) {
        this->active_time = active_time_opt.value();
    }
    return true;
}

}  // namespace primer
}  // namespace srp

