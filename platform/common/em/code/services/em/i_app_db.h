/**
 * @file i_app_db.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-12-30
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef PLATFORM_COMMON_EM_CODE_SERVICES_EM_I_APP_DB_H_
#define PLATFORM_COMMON_EM_CODE_SERVICES_EM_I_APP_DB_H_

#include <cstdint>
#include <functional>
#include <optional>
#include <string>
#include <unordered_set>

#include "platform/common/em/code/services/em/app_config.h"
namespace srp {
namespace em {
namespace service {
namespace data {
class IAppDb {
 public:
  /**
   * @brief Adding new App config to DB
   *
   * @param app New Config
   * @return int8_t 0 = OK
   */
  virtual int8_t InsertNewApp(AppConfig app) noexcept = 0;
  /**
   * @brief Adding new Functional group to DB
   *
   * @param fg_id functional group id
   * @param name functional group name
   * @return int8_t 0 = OK
   */
  virtual int8_t InsertNewFG(uint16_t fg_id,
                             const std::string& name) noexcept = 0;
  /**
   * @brief Set the Pid For App object
   *
   * @param app_id App ID from config file
   * @param pid new PID
   */
  virtual void SetPidForApp(const uint16_t app_id,
                            const uint32_t pid) noexcept = 0;
  /**
   * @brief Get the App Config object
   *
   * @param app_id_ App ID for which we want config
   * @return std::optional<std::reference_wrapper<const AppConfig>> const
   * reference to AppConfig
   */
  virtual std::optional<std::reference_wrapper<const AppConfig>> GetAppConfig(
      const uint16_t& app_id_) noexcept = 0;
  /**
   * @brief Get the Fg App List object
   *
   * @param fg_id functional group id
   * @return std::optional<
   * std::reference_wrapper<const std::unordered_set<uint16_t>>>
   */
  virtual std::optional<
      std::reference_wrapper<const std::unordered_set<uint16_t>>>
  GetFgAppList(const uint16_t& fg_id) noexcept = 0;
  virtual ~IAppDb() = default;
};

}  // namespace data
}  // namespace service
}  // namespace em
}  // namespace srp
#endif  // PLATFORM_COMMON_EM_CODE_SERVICES_EM_I_APP_DB_H_
