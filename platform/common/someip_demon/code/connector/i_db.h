/**
 * @file i_db.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef PLATFORM_COMMON_SOMEIP_DEMON_CODE_CONNECTOR_I_DB_H_
#define PLATFORM_COMMON_SOMEIP_DEMON_CODE_CONNECTOR_I_DB_H_

#include <cstdint>
#include <functional>
#include <optional>
#include <unordered_map>

#include "platform/common/someip_demon/code/db/find_service_item.h"

namespace srp {
namespace someip_demon {
namespace connectors {
class IDb {
 public:
  virtual ~IDb() = default;
  virtual std::optional<uint32_t> GetServiceProviderPid(
      const uint16_t service_id) const noexcept = 0;
  virtual std::optional<std::reference_wrapper<const db::FindServiceItem>>
  GetServiceIp(const uint16_t service_id,
               const uint32_t pid) const noexcept = 0;
};
}  // namespace connectors
}  // namespace someip_demon
}  // namespace srp
#endif  // PLATFORM_COMMON_SOMEIP_DEMON_CODE_CONNECTOR_I_DB_H_
