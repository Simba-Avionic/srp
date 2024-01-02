/**
 * @file method_element.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-12-11
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef COMMUNICATION_CORE_DATABASE_METHOD_ELEMENT_H_
#define COMMUNICATION_CORE_DATABASE_METHOD_ELEMENT_H_
#include <stdint.h>

#include <vector>

#include "database/service_element.h"

namespace simba {
namespace database {
namespace objects {
class MethodElement {
 private:
  const uint16_t method_id_;
  const ServiceElement service_;

 public:
  MethodElement(const uint16_t& method_id, const ServiceElement& service)
      : method_id_{method_id}, service_{service} {}
  uint16_t GetMethodId() const { return method_id_; }
  ServiceElement GetService() const { return service_; }
};
}  // namespace objects
}  // namespace database
}  // namespace simba
#endif  // COMMUNICATION_CORE_DATABASE_METHOD_ELEMENT_H_
