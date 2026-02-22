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
#include "apps/ec/engine_service/service.hpp"
#include "ara/log/log.h"

namespace srp {
namespace apps {
namespace service {

namespace {
    constexpr uint8_t OPEN_SERVO = 1;
    constexpr uint8_t CLOSE_SERVO = 0;
    constexpr uint16_t PRIMER_DELAY = 2000;
}


MyEngineServiceSkeleton::MyEngineServiceSkeleton(ara::core::InstanceSpecifier instance):
        EngineServiceSkeleton{instance}, primer_handler_{nullptr}, servo_handler_{nullptr},
        gpio_{std::make_unique<gpio::GPIOController>()} {
    state_ctr_ = engineApp::EngineStateController::GetInstance();
}
void MyEngineServiceSkeleton::Init(std::shared_ptr<PrimerServiceHandler> primer_handler,
                                    std::shared_ptr<ServoServiceHandler> servo_handler) {
    this->primer_handler_ = primer_handler;
    this->servo_handler_ = servo_handler;
}

ara::core::Result<bool> MyEngineServiceSkeleton::Start() {
    if ((primer_handler_ == nullptr || servo_handler_ == nullptr)) {
        ara::log::LogError() << "Invalid pointer to Primer or Servo";
        return ara::com::MakeErrorCode(
            ara::com::ComErrc::kWrongMethodCallProcessingMode, "Invalid pointer to Primer or Servo");
    }
    if (state_ctr_->GetEngineState() != RocketState_t::ARM) {
        ara::log::LogError() << "Invalid engine computer MODE";
        return ara::com::MakeErrorCode(
            ara::com::ComErrc::kWrongMethodCallProcessingMode, "Invalid engine computer MODE");
    }
    state_ctr_->SetEngineState(RocketState_t::LAUNCH);
    CurrentMode.Update(static_cast<uint8_t>(RocketState_t::LAUNCH));
    auto res = this->primer_handler_->OnPrime();
    if (!res.HasValue()) {
      ara::log::LogError() << "Invalid request to MW:GPIOService";
      return ara::com::MakeErrorCode(
            ara::com::ComErrc::kWrongMethodCallProcessingMode, "Invalid request to MW:GPIOService");
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(PRIMER_DELAY));
    auto res2 = this->servo_handler_->SetMainServoValue(OPEN_SERVO);
    this->primer_handler_->OffPrime();
    if (!res2.HasValue()) {
      ara::log::LogError() << "Invalid request to MW:I2CService";
      return ara::com::MakeErrorCode(
            ara::com::ComErrc::kWrongMethodCallProcessingMode, "Invalid request to MW:I2CService");
    }
    state_ctr_->SetEngineState(RocketState_t::FLIGHT);
    CurrentMode.Update(static_cast<uint8_t>(RocketState_t::FLIGHT));
    return ara::core::Result<bool>{true};
}

ara::core::Result<bool> MyEngineServiceSkeleton::SetMode(const std::uint8_t& in_parm) {
    auto old_state = state_ctr_->GetEngineState();
    if (!this->state_ctr_->SetEngineState(static_cast<RocketState_t>(in_parm))) {
        ara::log::LogError() << "Try to do illegal transition from: "
                        << RocketState::GetStringState(old_state) <<
                        ", to: " <<RocketState::GetStringState(static_cast<RocketState_t>(in_parm));
        return ara::core::Result<bool>{false};
    }

    CurrentMode.Update(static_cast<uint8_t>(in_parm));
    ara::log::LogInfo() << "Change Mode from: " << RocketState::GetStringState(old_state) <<
                        ", to: " <<RocketState::GetStringState(static_cast<RocketState_t>(in_parm));
    return ara::core::Result<bool>{true};
}

}  // namespace service
}  // namespace apps
}  // namespace srp
