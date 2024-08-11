/**
 * @file Iapplication.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief Interface for all Simba application
 * @version 1.0
 * @date 2023-10-23
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef CORE_APPLICATION_IAPPLICATION_H_
#define CORE_APPLICATION_IAPPLICATION_H_
#include <stop_token>
#include <string>
#include <unordered_map>

#include "core/common/error_code.h"
namespace simba {
namespace core {
class IApplication {
 protected:
  /**
   * @brief This function is called to launch the application
   *
   * @param token stop token
   */
  virtual ErrorCode Run(const std::stop_token& token) = 0;
  /**
   * @brief This function is called to initialiaze the application
   *
   * @param parms map with parms
   */
  virtual ErrorCode Initialize(
      const std::unordered_map<std::string, std::string>& parms) = 0;
  /**
   * @brief This is pre-run function only for creting new application
   * interfacess
   *
   * @param parms map with parms
   */
  virtual void onRun(const std::unordered_map<std::string, std::string>& parms) = 0;

 public:
  virtual void StopApp() = 0;
  virtual ~IApplication() = default;
  virtual void RunApp(int argc, char const* argv[]) = 0;
};
}  // namespace core
}  // namespace simba
#endif  // CORE_APPLICATION_IAPPLICATION_H_
