/**
 * @file ptp_controller.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-25
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "core/ptp/ptp_controller/ptp_controller.hpp"
#include "ara/core/instance_specifier.h"
#include "ara/log/log.h"
#include <chrono>  // NOLINT
#include <thread>  // NOLINT
namespace simba {
namespace ptp {

namespace {
  constexpr auto kShm_name = "global_timestamp";
}  // namespace

PTPController::PTPController(): shm_(ara::core::InstanceSpecifier{kShm_name}) {
}

int PTPController::Init() {
    auto res = shm_.OfferService();
    if (!res.HasValue()) {
        ara::log::LogWarn() << res.Error();
    }
    auto res2 = shm_.Send(std::chrono::high_resolution_clock::now());
    if (!res2.HasValue()) {
        ara::log::LogWarn() << res2.Error();
    }
    // TODO(matikrajek42@gmail.com) add starting PTP thread to sync timestamp beatween devices
    return 0;
}
PTPController::~PTPController() {
}

}  // namespace ptp
}  // namespace simba
