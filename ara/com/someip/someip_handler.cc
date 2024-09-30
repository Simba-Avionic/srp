/**
 * @file someip_handler.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-09-15
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "ara/com/someip/someip_handler.h"

#include <future>  // NOLINT

#include "ara/com/com_error_domain.h"
#include "ara/com/log.h"
#include "ara/com/someip/bindings/binding_factory.h"
#include "ara/com/someip/message_code.h"
#include "ara/com/someip/message_type.h"
#include "ara/core/model/com_model_item.h"
#include "ara/core/model_db.h"

namespace ara {
namespace com {
namespace someip {
ara::core::Result<void> ServiceHandler::StartFind(std::stop_token token) {
  const auto conf_model =
      ara::core::ModelDataBase::GetInstance()
          .ResolveInstanceSpecifier<ara::core::model::ModelSomeIp>(
              this->instance_);
  if (!conf_model.HasValue()) {
    ara::com::LogError() << "Model Error";
    return conf_model.Error();
  }
  const auto& conf_model_v = conf_model.Value();
  this->bindings_ =
      ara::com::someip::bindings::BindingsFactory::GetBindings(conf_model_v);
  this->bindings_->SetServiceData(this->service_id_, conf_model_v);

  ara::com::LogInfo() << "Mapping: [" << instance_
                      << "] map as service_id: " << service_id_
                      << " instance: " << conf_model_v.instance_;
  this->bindings_->SetProxyCallback(
      std::bind(&ServiceHandler::HandleEvent, this, std::placeholders::_1,
                std::placeholders::_2),
      std::bind(&ServiceHandler::SubscribeNewStatus, this,
                std::placeholders::_1, std::placeholders::_2));
  this->bindings_->Start(token);
  this->founded_ = !token.stop_requested();
  return {};
}
void ServiceHandler::SubscribeEvent(const uint16_t& event_id_) {
  std::async(std::launch::async, [this, &event_id_]() {
    this->bindings_->SubscribeToEvent(event_id_);
  });
}

ara::core::Result<void> ServiceHandler::StopFind() {}
ServiceHandler::ServiceHandler(const ara::core::InstanceSpecifier& instance,
                               uint16_t service_id)
    : instance_{instance}, service_id_{service_id} {}
}  // namespace someip
}  // namespace com
}  // namespace ara
