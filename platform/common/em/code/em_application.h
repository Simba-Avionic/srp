/**
 * @file em_application.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-02-06
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef PLATFORM_COMMON_EM_CODE_EM_APPLICATION_H_
#define PLATFORM_COMMON_EM_CODE_EM_APPLICATION_H_


#include <condition_variable>  // NOLINT
#include <map>
#include <memory>
#include <queue>
#include <string>

#include "ara/exec/adaptive_application.h"
#include "platform/common/em/code/services/em/em_service.h"
namespace simba {
namespace em {
class EmApplication final : public ara::exec::AdaptiveApplication {
 private:
  /**
   * @brief This function is called to initialiaze the application
   *
   * @param parms map with parms
   */
  int Initialize(const std::map<ara::core::StringView, ara::core::StringView>
                     parms) override;
  /**
   * @brief This function is called to launch the application
   *
   * @param token stop token
   */
  int Run(const std::stop_token& token) override;

  service::EmService em_service{};

 public:
  EmApplication(/* args */);
  ~EmApplication();
};

}  // namespace em
}  // namespace simba

#endif  // PLATFORM_COMMON_EM_CODE_EM_APPLICATION_H_
