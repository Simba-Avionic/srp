/**
 * @file em_service.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-02-06
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef MW_EM_CODE_SERVICES_EM_EM_SERVICE_H_
#define MW_EM_CODE_SERVICES_EM_EM_SERVICE_H_
#include <string>
#include <unordered_map>
#include <vector>
#include <optional>

#include "app_config.h"

namespace simba {
namespace em {
namespace service {
class EmService {
 private:
  std::vector<std::uint8_t> app_level_list{};
  std::vector<data::AppConfig> app_list{};
  bool IsSrpApp(const std::string& path) noexcept;
  std::optional<data::AppConfig> GetAppConfig(const std::string& path) noexcept;

 public:
  std::vector<service::data::AppConfig> GetAppList() {
    return this->app_list;
  }
  void LoadApps() noexcept;
  void StartApps() noexcept;
  void StartApps(const uint8_t level) noexcept;
  void RestartApp(const uint16_t appID);
  EmService(/* args */);
  ~EmService();
};

}  // namespace service
}  // namespace em
}  // namespace simba
#endif  // MW_EM_CODE_SERVICES_EM_EM_SERVICE_H_
