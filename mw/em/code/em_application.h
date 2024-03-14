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
#ifndef MW_EM_CODE_EM_APPLICATION_H_
#define MW_EM_CODE_EM_APPLICATION_H_

#include <string>
#include <unordered_map>
#include <queue>
#include <memory>

#include "core/application/application_mw.h"
#include "mw/em/code/services/em/em_service.h"
#include "mw/em/code/services/exec/exec_manager.hpp"
namespace simba {
namespace em {
class EmApplication final : public core::ApplicationMW {
 private:
  /**
   * @brief This function is called to launch the application
   *
   * @param token stop token
   */
  core::ErrorCode Run(std::stop_token token) final;
  /**
   * @brief This function is called to initialize the application
   *
   * @param parms map with parms
   */
  core::ErrorCode Initialize(
      const std::unordered_map<std::string, std::string>& parms) final;

  std::queue<uint16_t> restartQueue;
  service::EmService em_service{};
  mw::exec::ExecManager exec_service{std::make_shared<std::queue<uint16_t>>(this->restartQueue)};

 public:
  EmApplication(/* args */);
  ~EmApplication();
};

}  // namespace em
}  // namespace simba
#endif  // MW_EM_CODE_EM_APPLICATION_H_
