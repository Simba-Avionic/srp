/**
 * @file database.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-02-24
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef COMMUNICATION_CORE_SOMEIP_DATABASE_CODE_DATABASE_H_
#define COMMUNICATION_CORE_SOMEIP_DATABASE_CODE_DATABASE_H_
#include <optional>
#include <unordered_map>
#include <vector>
#include "communication-core/someip-database/code/interface.h"
#include "communication-core/someip-database/code/service.h"
#include "core/common/error_code.h"
namespace simba {
namespace com {
namespace someip {
namespace objects {
class DataBase {
 private:
  std::unordered_map<uint16_t, Interface> list{};
  std::unordered_map<uint16_t, std::vector<uint16_t>> event_map{};
 public:
  core::ErrorCode InsertService(const uint16_t service_id,
                                const Interface& inf) noexcept;
  std::optional<Interface> FindService(
      const uint16_t service_id) const noexcept;

  core::ErrorCode InstertEvent(const uint16_t event_id, const uint16_t client_id) noexcept;
  std::optional<std::vector<uint16_t>> FindEventClient(const uint16_t event_id) const noexcept;
};

}  // namespace objects
}  // namespace someip
}  // namespace com
}  // namespace simba

#endif  // COMMUNICATION_CORE_SOMEIP_DATABASE_CODE_DATABASE_H_
