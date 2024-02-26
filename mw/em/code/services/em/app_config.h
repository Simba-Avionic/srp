/**
 * @file app_config.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-02-06
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef MW_EM_CODE_SERVICES_EM_APP_CONFIG_H_
#define MW_EM_CODE_SERVICES_EM_APP_CONFIG_H_

#include <cstdint>
#include <string>

namespace simba {
namespace em {
namespace service {
namespace data {
class AppConfig {
 private:
  const std::string bin_path_;
  const std::string parms_;
  const uint8_t startup_prio_;
  const uint8_t startup_after_delay_;

 public:
  AppConfig(const std::string &bin_path, const std::string &parms,
            const uint8_t startup_prio, const uint8_t startup_after_delay)
      : bin_path_{bin_path},
        parms_{parms},
        startup_prio_{startup_prio},
        startup_after_delay_{startup_after_delay} {}

  std::string GetBinPath() const { return bin_path_; }
  std::string GetParms() const { return parms_; }
  uint8_t GetStartUpPrio() const { return startup_prio_; }
  uint8_t GetStartUpAfterDelay() const { return startup_after_delay_; }
};
}  // namespace data
}  // namespace service
}  // namespace em
}  // namespace simba

#endif  // MW_EM_CODE_SERVICES_EM_APP_CONFIG_H_
