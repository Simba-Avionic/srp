/**
 * @file events_element.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-12-11
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef COMMUNICATION_CORE_DATABASE_EVENT_ELEMENT_H_
#define COMMUNICATION_CORE_DATABASE_EVENT_ELEMENT_H_
#include <stdint.h>

#include <vector>

#include "database/service_element.h"

namespace simba {
namespace database {
namespace objects {
class EventElement {
 private:
  const uint16_t event_id_;
  std::vector<ServiceElement> client_lists_{};

 public:
  explicit EventElement(const uint16_t& event_id) : event_id_{event_id} {}
  void AddService(const ServiceElement& item) { client_lists_.push_back(item); }
  const std::vector<ServiceElement>& GetLists() const {
    return this->client_lists_;
  }
};
}  // namespace objects
}  // namespace database
}  // namespace simba
#endif  // COMMUNICATION_CORE_DATABASE_EVENT_ELEMENT_H_
