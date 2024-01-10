/**
 * @file req_event_element.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-01-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef COMMUNICATION_CORE_DATABASE_REQ_EVENT_ELEMENT_H_
#define COMMUNICATION_CORE_DATABASE_REQ_EVENT_ELEMENT_H_
#include <stdint.h>

namespace simba {
namespace database {
namespace objects {
class ReqEventElement {
 private:
  const uint16_t event_id_;
  const uint16_t service_id;

 public:
  ReqEventElement(const uint16_t& event_id, const uint16_t& service_id)
      : event_id_{event_id}, service_id{service_id} {}
};
}  // namespace objects
}  // namespace database
}  // namespace simba
#endif  // COMMUNICATION_CORE_DATABASE_REQ_EVENT_ELEMENT_H_
