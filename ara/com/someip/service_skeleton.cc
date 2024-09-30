/**
 * @file service_skeleton.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-09-15
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "ara/com/someip/service_skeleton.h"

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

ServiceSkeleton::ServiceSkeleton(const ::ara::core::InstanceSpecifier& instance,
                                 uint16_t service_id)
    : instance_{instance}, service_id_{service_id} {}

void ServiceSkeleton::Handler(const uint16_t& method_id,
                              const std::vector<uint8_t>& payload) {
  std::ignore = this->HandleMethod(method_id, payload);
}

ara::core::Result<std::vector<uint8_t>> ServiceSkeleton::HandlerWithResult(
    const uint16_t& method_id, const std::vector<uint8_t>& payload) {
  return this->HandleMethod(method_id, payload);
}

ServiceSkeleton::~ServiceSkeleton() {}

void ServiceSkeleton::HandleEvent(const uint16_t event_id,
                                  const std::vector<uint8_t>& data) {
  bindings_->HandleEvent(event_id, data);
}

ara::core::Result<void> ServiceSkeleton::StartOffer() {
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
  this->bindings_->SetSkeletonCallback(std::bind(&ServiceSkeleton::HandleMethod,
                                                 this, std::placeholders::_1,
                                                 std::placeholders::_2));
  this->BinderReady();

  ara::com::LogInfo() << "Mapping: [" << instance_
                      << "] map as service_id: " << service_id_
                      << " instance: " << conf_model_v.instance_;
  std::stop_source s{};
  this->bindings_->Start(s.get_token());
  return {};
}

ara::core::Result<void> ServiceSkeleton::StopOffer() {
  if (bindings_ == nullptr) {
    return ara::com::MakeErrorCode(ara::com::ComErrc::kServiceNotOffered,
                                   "Service Not offered");
  }
  this->bindings_->Stop();
  return {};
}
}  // namespace someip
}  // namespace com
}  // namespace ara
