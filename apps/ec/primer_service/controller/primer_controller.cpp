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
  static constexpr uint16_t kIgniter_active_time = 2500;
  static constexpr uint8_t kPrimer_on = 1;
  static constexpr uint8_t kPrimer_off = 0;
  static constexpr bool kRequire_primer_veryfication = false;
  static constexpr auto kIgniter_measureme_adc_channel = 4;
  static constexpr auto kIgniter_measurement_adc_samples = 5;
  static constexpr auto kIgniter_connected_treshold_mV = 1500;

const std::string PrimerStateToString(const PrimerState_t state) {
    switch (state) {
        case PrimerState_t::kUNKNOWN:
            return "UNKNOWN";
        case PrimerState_t::kNOT_CONNECTED:
            return "NOT_CONNECTED";
        case PrimerState_t::kCONNECTED:
            return "CONNECTED";
        case PrimerState_t::kSHORT_CIRCUIT:
            return "SHORT_CIRCUIT";
        case PrimerState_t::kFIRED:
            return "FIRED";
        default:
            return "INVALID";
    }
}
}  // namespace

PrimerController::PrimerController():
            gpio_(std::make_unique<com::soc::StreamIpcSocket>()),
            prim_logger{ara::log::LoggingMenager::GetInstance()->CreateLogger(
          "CTRL", "", ara::log::LogLevel::kWarn)} {
    prim_logger.LogInfo() << "PrimerController initialized";
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
        prim_logger.LogWarn() << "Primer verification skipped: ADC returned no valid samples";
        return;
    }
    auto mean = sum / static_cast<float>(sample_num);
    const auto previous_state = primerState.load();
    auto next_state = previous_state;
    if (mean >= kIgniter_connected_treshold_mV) {
        next_state = PrimerState_t::kCONNECTED;
    } else {
        next_state = PrimerState_t::kNOT_CONNECTED;
    }

    primerState.store(next_state);
    if (previous_state != next_state) {
        prim_logger.LogInfo() << "Primer state changed from " << PrimerStateToString(previous_state)
                              << " to " << PrimerStateToString(next_state)
                              << " (ADC mean: " << mean << " mV)";
    }
}

void PrimerController::Initialize() {
    prim_logger.LogInfo() << "Initializing PrimerController";
    this->primerState = PrimerState_t::kUNKNOWN;
    this->adc_.Init();
    prim_logger.LogInfo() << "PrimerController initialization complete";
}

bool PrimerController::DisablePrimer() {
    if (primerState != PrimerState_t::kFIRED) {
        prim_logger.LogWarn() << "DisablePrimer rejected: primer is in state "
                              << PrimerStateToString(primerState.load());
        return false;
    }
    if (this->gpio_.SetPinValue(kIgniter_pin_id, kPrimer_off, 0) != core::ErrorCode::kOk) {
        prim_logger.LogError() << "Failed to disable primer";
        return false;
    }
    prim_logger.LogInfo() << "Primer output disabled";
    return true;
}

bool PrimerController::EnablePrimer(const bool auto_disable) {
    prim_logger.LogInfo() << "EnablePrimer request received (auto_disable=" << auto_disable << ")";
    if (kRequire_primer_veryfication && primerState != PrimerState_t::kCONNECTED) {
        prim_logger.LogWarn() << "EnablePrimer rejected: primer verification enabled and state is "
                              << PrimerStateToString(primerState.load());
        return false;
    }
    if (this->gpio_.SetPinValue(kIgniter_pin_id, kPrimer_on, kIgniter_active_time) != core::ErrorCode::kOk) {
        prim_logger.LogError() << "Failed to enable primer";
        return false;
    }
    primerState = PrimerState_t::kFIRED;
    prim_logger.LogInfo() << "Primer fired (active_time_ms=" << kIgniter_active_time << ")";
    return true;
}
PrimerState_t PrimerController::GetPrimerState() const noexcept {
    return primerState;
}

}  // namespace primer
}  // namespace srp

