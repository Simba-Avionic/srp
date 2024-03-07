/**
 * @file application_common.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-02-02
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef CORE_APPLICATION_APPLICATION_COMMON_H_
#define CORE_APPLICATION_APPLICATION_COMMON_H_

#include <chrono>      // NOLINT
#include <stop_token>  // NOLINT
#include <string>
#include <thread>  // NOLINT
#include <unordered_map>

#include "core/application/Iapplication.h"
#include "diag/exec/controller/exec_controller.hpp"
namespace simba {
namespace core {
class ApplicationCommon : public IApplication {
 protected:
  std::stop_source source;
  diag::exec::ExecController exec_;
  /**
   * @brief This is pre-run function only for creting new application
   * interfacess
   *
   * @param parms map with parms
   */
  void onRun(
      const std::unordered_map<std::string, std::string>& parms) override;
  void SleepMainThread();
  ErrorCode CommonConfig(
      const std::unordered_map<std::string, std::string>& parms);
  bool FileExist(const std::string& name);

 public:
  void StopApp() final;
  void RunApp(int argc, char const* argv[]) final;
  virtual ~ApplicationCommon();
};

}  // namespace core
}  // namespace simba
#endif  // CORE_APPLICATION_APPLICATION_COMMON_H_
