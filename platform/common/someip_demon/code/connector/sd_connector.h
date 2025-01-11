/**
 * @file sd_connector.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-11-26
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef PLATFORM_COMMON_SOMEIP_DEMON_CODE_CONNECTOR_SD_CONNECTOR_H_
#define PLATFORM_COMMON_SOMEIP_DEMON_CODE_CONNECTOR_SD_CONNECTOR_H_
#include <memory>
#include <memory_resource>
#include <string>
#include <thread>  // NOLINT
#include <unordered_map>
#include <utility>
#include <vector>

#include "bindings/common/socket/proccess_socket.h"
#include "ara/com/someip/ServiceEntry.h"
#include "ara/com/someip/someip_frame.h"
#include "platform/common/someip_demon/code/common/udp_socket.h"
#include "platform/common/someip_demon/code/connector/common_connector.h"
#include "platform/common/someip_demon/code/connector/i_db.h"
#include "platform/common/someip_demon/code/db/database.h"
namespace srp {
namespace someip_demon {
namespace connectors {
class SDConnector : public CommonConnector, public IDb {
 public:
  struct SdProxy {
    bool provided_{false};
    std::reference_wrapper<const db::FindServiceItem> sd_object_;
  };

 private:
  const std::uint32_t local_ip_;
  const std::uint16_t port_;
  db::Database& sd_db_;
  std::unordered_map<uint16_t, uint32_t> provider_service_list_{};
  std::pmr::unordered_map<uint32_t, std::pmr::unordered_map<uint16_t, SdProxy>>
      consume_service_list_{std::pmr::new_delete_resource()};
  std::unique_ptr<std::jthread> sd_thread_;
  void AddNewOfferService(const ara::com::someip::ServiceEntry& entry,
                          const uint32_t pid);
  void AddNewFindService(const ara::com::someip::ServiceEntry& entry,
                         const uint32_t pid);
  void SdMsgHandler(const uint32_t pid,
                    const ara::com::someip::SomeipFrame& frame);
  void SdCallback(std::reference_wrapper<db::FindServiceItem> item);

  void SdLoop(std::stop_token token);
  void SendIPCSdOffer(uint32_t pid,
                      std::reference_wrapper<const db::FindServiceItem> item);

 public:
  void ProcessFrame(uint32_t pid,
                    ara::com::someip::SomeipFrame&& frame) noexcept override;
  void ProcessFrame(const std::string& ip, std::uint16_t port,
                    ara::com::someip::SomeipFrame&& frame) noexcept override;
  void Start() noexcept override;
  std::optional<uint32_t> GetServiceProviderPid(
      const uint16_t service_id) const noexcept override;

  std::optional<std::reference_wrapper<const db::FindServiceItem>> GetServiceIp(
      const uint16_t service_id, const uint32_t pid) const noexcept override;

  explicit SDConnector(db::Database& sd_db, std::uint32_t local_ip,  // NOLINT
                       std::uint16_t port);
  ~SDConnector() = default;
};
}  // namespace connectors
}  // namespace someip_demon
}  // namespace srp
#endif  // PLATFORM_COMMON_SOMEIP_DEMON_CODE_CONNECTOR_SD_CONNECTOR_H_
