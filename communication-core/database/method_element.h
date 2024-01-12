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

namespace simba {
namespace database {
namespace objects {
class MethodElement {
 private:
  const uint16_t method_id_;
  const uint16_t service_id_;

 public:
  MethodElement(const uint16_t& method_id, const uint16_t& service)
      : method_id_{method_id}, service_id_{service} {}
  uint16_t GetMethodId() const { return method_id_; }
  uint16_t GetService() const { return service_id_; }
};
}  // namespace objects
}  // namespace database
}  // namespace simba
#endif  // COMMUNICATION_CORE_DATABASE_METHOD_ELEMENT_H_
