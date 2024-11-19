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


MyEngineServiceSkeleton::MyEngineServiceSkeleton(const ara::core::InstanceSpecifier& instance):
        EngineServiceSkeleton{instance} {
    this->primer_handler_ = nullptr;
    this->servo_handler_ = nullptr;
    CurrentMode.Update(static_cast<uint8_t>(mode_));
}
  void MyEngineServiceSkeleton::Init(const std::shared_ptr<PrimerServiceHandler>& primer_handler,
                                    const std::shared_ptr<ServoServiceHandler>& servo_handler) {
    this->primer_handler_ = primer_handler;
    this->servo_handler_ = servo_handler;
}

ara::core::Result<bool> MyEngineServiceSkeleton::Start() {
    if ((primer_handler_ == nullptr || servo_handler_ == nullptr)) {
        return ara::com::MakeErrorCode(
            ara::com::ComErrc::kWrongMethodCallProcessingMode, "Invalid pointer to Primer or Servo");
    }
    if (mode_ != MODE_t::AUTO) {
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
    this->mode_ = static_cast<MODE_t>(in_parm);
    CurrentMode.Update(static_cast<uint8_t>(mode_));
    return true;
}

}  // namespace service
}  // namespace apps
}  // namespace simba
