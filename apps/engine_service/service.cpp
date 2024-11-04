/**
 * @file service.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-04
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <memory>

#include "apps/engine_service/service.hpp"

namespace simba {
namespace apps {
namespace service {

namespace {
    constexpr uint8_t OPEN_SERVO = 1;
    constexpr uint8_t CLOSE_SERVO = 0;
    constexpr uint16_t PRIMER_DELAY = 200;
}


MyEngineServiceSkeleton::MyEngineServiceSkeleton(const ara::core::InstanceSpecifier& instance,
        std::shared_ptr<PrimerServiceHandler> primer_handler, std::shared_ptr<ServoServiceHandler> servo_handler,
        std::shared_ptr<MODE_t> mode): EngineServiceSkeleton{instance}, primer_handler_(primer_handler),
        servo_handler_(servo_handler), mode_(mode) {
    CurrentMode.Update(static_cast<uint8_t>(*mode_));
}


ara::core::Result<bool> MyEngineServiceSkeleton::Start() {
    if (*mode_ != MODE_t::AUTO) {
        return ara::com::MakeErrorCode(
            ara::com::ComErrc::kWrongMethodCallProcessingMode, "Invalid engine computer MODE");
    }
    auto res = this->servo_handler_->SetMainServoValue.Call(1);
    if (!res.HasValue()) {
      return ara::com::MakeErrorCode(
            ara::com::ComErrc::kWrongMethodCallProcessingMode, "Invalid request to MW:I2CService");
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(PRIMER_DELAY));
    auto res2 = this->primer_handler_->OnPrime.Call();
    if (!res.HasValue()) {
      return ara::com::MakeErrorCode(
            ara::com::ComErrc::kWrongMethodCallProcessingMode, "Invalid request to MW:GPIOService");
    }
    return true;
}

ara::core::Result<bool> MyEngineServiceSkeleton::SetMode(const std::uint8_t& in_parm) {
    auto mode = static_cast<MODE_t>(in_parm);
    CurrentMode.Update(static_cast<uint8_t>(*mode_));
    return true;
}

}  // namespace service
}  // namespace apps
}  // namespace simba
