/**
 * @file binding_factory.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-09-15
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "ara/com/someip/bindings/binding_factory.h"

#include <memory>

#include "ara/com/someip/bindings/ipc_bindings.h"
namespace ara {
namespace com {
namespace someip {
namespace bindings {
std::shared_ptr<ara::com::someip::bindings::IBindings>
BindingsFactory::GetBingings(const ara::core::model::ModelSomeIp& model) {
  if (model.dir_ == ara::core::model::ModelSomeIp::Direction::kOut) {
    if (model.port_ == 0) {
      return std::make_shared<IPCSkeletonBindings>(model.com_id_);
    } else {
    }
  } else if (model.dir_ == ara::core::model::ModelSomeIp::Direction::kIn) {
    return std::make_shared<IPCProxyBindings>(model.com_id_);
  }
}
}  // namespace bindings
}  // namespace someip
}  // namespace com
}  // namespace ara
