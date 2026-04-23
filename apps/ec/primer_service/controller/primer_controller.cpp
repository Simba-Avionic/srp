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
  static constexpr uint8_t kIgniter_pin_id = 13;
  static constexpr uint16_t kIgniter_active_time = 1000;
  static constexpr uint8_t kPrimer_on = 1;
  static constexpr bool kRequire_primer_veryfication = true;
  static constexpr auto kIgniter_measureme_adc_channel = 4;
  static constexpr auto kIgniter_measurement_adc_samples = 5;
  static constexpr auto kIgniter_connected_treshold_mV = 1500;
}  // namespace

PrimerController::PrimerController():
            gpio_(std::make_unique<com::soc::StreamIpcSocket>()) {
}

void PrimerController::VerifyPrimerConection() {
    auto verify_required = [this]() {
        auto primer_state = primerState.load();
        return (primer_state == PrimerState_t::kUNKNOWN || primer_state == PrimerState_t::kNOT_CONNECTED ||
            primer_state == PrimerState_t::kCONNECTED || primer_state == PrimerState_t::kSHORT_CIRCUIT);
    };
    if (!verify_required()) {
        return;
    }
    float sum = 0;
    auto sample_num = 0;
    for (int i = 0; i < kIgniter_measurement_adc_samples; i++) {
        auto voltage = adc_.GetAdcVoltage(kIgniter_measureme_adc_channel);
        if (!voltage.has_value()) {
            continue;
        }
        sum += voltage.value();
        sample_num += 1;
    }
    if (sample_num == 0) {
        return;
    }
    auto mean = sum / static_cast<float>(sample_num);
    ara::log::LogWarn() << "Read value: " << std::to_string(mean);
    if (mean >= kIgniter_connected_treshold_mV) {
        primerState.store(PrimerState_t::kCONNECTED);
    } else {
        primerState.store(PrimerState_t::kNOT_CONNECTED);
    }
}

void PrimerController::Initialize(std::string path) {
    this->primerState = PrimerState_t::kUNKNOWN;
    this->ReadConfig(path);
    this->adc_.Init();
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
    return true;
}

bool PrimerController::EnablePrimer(const bool auto_disable) {
    if (kRequire_primer_veryfication & primerState != PrimerState_t::kCONNECTED) {
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

