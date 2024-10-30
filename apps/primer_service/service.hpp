/**
 * @file service.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-10-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef APPS_PRIMER_SERVICE_SERVICE_HPP_
#define APPS_PRIMER_SERVICE_SERVICE_HPP_

#include <memory>
#include <utility>
#include <vector>
#include "simba/apps/PrimerServiceSkeleton.h"
#include "apps/primer_service/controller/primer_controller.hpp"

namespace simba {
namespace apps {

class MyPrimerServiceSkeleton: public PrimerServiceSkeleton {
 private:
    std::shared_ptr<primer::PrimerController> controller_;
 public:
  MyPrimerServiceSkeleton(const ara::core::InstanceSpecifier& instance,
        std::shared_ptr<primer::PrimerController> controller):
                PrimerServiceSkeleton{instance}, controller_(std::move(controller)) {
    primeStatusEvent.SetCallback(std::bind(&MyPrimerServiceSkeleton::HandleEvent,
        this, std::placeholders::_1, std::placeholders::_2));
  }
 protected:
  ara::core::Result<bool> OnPrime() override {
  return ara::core::Result<bool>(controller_->ChangePrimerState(1));
  }
  ara::core::Result<bool> OffPrime() override {
  return ara::core::Result<bool>(controller_->ChangePrimerState(0));
  }
  ara::core::Result<bool> StartPrime() override {
  return ara::com::MakeErrorCode(ara::com::ComErrc::kSetHandlerNotSet, "Method handler is not set");
  }
};
}  // namespace apps
}  // namespace simba

#endif  // APPS_PRIMER_SERVICE_SERVICE_HPP_
