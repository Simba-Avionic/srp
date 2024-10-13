#ifndef PLATFORM_COMMON_SOMEIP_DEMON_CODE_DB_FIND_SERVICE_ITEM_H_
#define PLATFORM_COMMON_SOMEIP_DEMON_CODE_DB_FIND_SERVICE_ITEM_H_

#include <cstdint>

namespace simba {
namespace someip_demon {
namespace db {
class FindServiceItem {
 private:
 public:
  const uint16_t service_id_;
  const uint16_t instance_id_;
  const std::uint8_t major_version_;
  const std::uint32_t minor_version_;
  std::uint32_t ip_;
  std::uint16_t port_;
  FindServiceItem(const uint16_t service_id, const uint16_t instance_id,
              const std::uint8_t major_version,
              const std::uint32_t minor_version)
      : service_id_{service_id},
        instance_id_{instance_id},
        major_version_{major_version},
        minor_version_{minor_version} {}
  ~FindServiceItem() = default;
};
}  // namespace db
}  // namespace someip_demon
}  // namespace simba

#endif  // PLATFORM_COMMON_SOMEIP_DEMON_CODE_DB_FIND_SERVICE_ITEM_H_
