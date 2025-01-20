/**
 * @file service.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-02-24
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef COMMUNICATION_CORE_SOMEIP_DATABASE_CODE_SERVICE_H_
#define COMMUNICATION_CORE_SOMEIP_DATABASE_CODE_SERVICE_H_
#include <cstdint>
#include <vector>

#include "communication-core/someip-database/code/interface.h"
namespace srp {
namespace com {
namespace someip {
namespace objects {
class Service {
 private:
  const uint16_t service_id_;
  std::vector<Interface> interfaces_;

 public:
  Service(const uint16_t service_id, const std::vector<Interface>& interfaces)
      : service_id_{service_id}, interfaces_{interfaces} {}
  uint16_t GetServiceId() const { return service_id_; }
  const std::vector<Interface>& GetInterfacesList() const {
    return interfaces_;
  }
};
}  // namespace objects
}  // namespace someip
}  // namespace com
}  // namespace srp

#endif  // COMMUNICATION_CORE_SOMEIP_DATABASE_CODE_SERVICE_H_
