/**
 * @file binding_factory.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-09-15
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef ARA_COM_SOMEIP_BINDINGS_BINDING_FACTORY_H_
#define ARA_COM_SOMEIP_BINDINGS_BINDING_FACTORY_H_

#include <cstdint>
#include <functional>
#include <memory>
#include <vector>

#include "ara/com/someip/bindings/i_bindings.h"
#include "ara/core/model/com_model_item.h"
namespace ara {
namespace com {
namespace someip {
namespace bindings {
class BindingsFactory {
 public:
  static std::shared_ptr<ara::com::someip::bindings::IBindings> GetBindings(
      const ara::core::model::ModelSomeIp& model);
};
}  // namespace bindings
}  // namespace someip
}  // namespace com
}  // namespace ara

#endif  // ARA_COM_SOMEIP_BINDINGS_BINDING_FACTORY_H_
