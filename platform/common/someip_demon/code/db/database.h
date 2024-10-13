#ifndef PLATFORM_COMMON_SOMEIP_DEMON_CODE_DB_DATABASE_H_
#define PLATFORM_COMMON_SOMEIP_DEMON_CODE_DB_DATABASE_H_

#include <memory_resource>
#include <unordered_map>
#include <unordered_set>

#include "platform/common/someip_demon/code/db/service_item.h"
#include "platform/common/someip_demon/code/db/find_service_item.h"
namespace simba {
namespace someip_demon {
namespace db {
class Database {
 public:
  // List with service withch are provide by this ECU
  using OfferedServiceTable = std::pmr::unordered_map<uint32_t, ServiceItem>;
  // List with Service withch are consume by this ECU
  using FindServiceTable = std::pmr::unordered_map<uint32_t, FindServiceItem>;
  // List with Service witch we need
  using ReqServiceTable =
      std::pmr::unordered_map<uint32_t, std::pmr::unordered_set<uint32_t>>;

 private:
  OfferedServiceTable offered_service_;
  FindServiceTable find_service_;
  ReqServiceTable req_table_;
 public:
  Database(/* args */)
      : offered_service_{std::pmr::new_delete_resource()},
        find_service_{std::pmr::new_delete_resource()} {}
  ~Database() = default;

  bool AddNewProvideService(const ServiceItem& item) {
    return AddService(offered_service_, item);
  }

  bool AddNewConsumeService(const FindServiceItem& item) {
    return AddService(find_service_, item);
  }

  template <typename TableSection, typename ItemType>
  bool AddService(TableSection& section, const ItemType& item) {
    const uint32_t key{(static_cast<uint32_t>(item.instance_id_) << 16) +
                       item.service_id_};
    return section.insert({key, item}).second;
  }

  const OfferedServiceTable& GetProvideList() const { return offered_service_; }
  const FindServiceTable& GetConsumeList() const { return find_service_; }
};
}  // namespace db
}  // namespace someip_demon
}  // namespace simba

#endif  // PLATFORM_COMMON_SOMEIP_DEMON_CODE_DB_DATABASE_H_
