/**
 * @file initialization.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-11-26
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "ara/com/initialization.h"

#include "ara/com/com_controller.h"
#include "ara/com/someip/controller/someip_controller.h"
#include "ara/core/result.h"
#include "ara/diag/diagnostic_menager.h"
namespace ara {
namespace com {
ara::core::Result<void> Initialize(const uint32_t& app_id) noexcept {
  auto& controller = ara::com::ComController::GetInstance(app_id);
  controller.AddHandler(IComClient::MsgType::kSomeIp,
                        ara::com::someip::SomeipController::GetInstance());
  controller.AddHandler(IComClient::MsgType::kDiag,
                        ara::diag::DiagnosticMenager::GetInstance());
  controller.Init();
  return {};
}

ara::core::Result<void> Deinitialize() noexcept { return {}; }
}  // namespace com
}  // namespace ara
