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
#include <unordered_map>

#include "communication-core/someip-database/code/interface.h"
#include "core/common/error_code.h"
#include "core/results/result.h"
namespace simba {
namespace com {
namespace someip {
namespace objects {
class DataBase {
 private:
  std::unordered_map<uint16_t, Interface> list{};

 public:
  core::ErrorCode InsertService(const uint16_t service_id,
                                const Interface& inf) noexcept;
  core::Result<Interface> FindService(const uint16_t service_id) const noexcept;
};

}  // namespace objects
}  // namespace someip
}  // namespace com
}  // namespace simba

#endif  // COMMUNICATION_CORE_SOMEIP_DATABASE_CODE_DATABASE_H_
