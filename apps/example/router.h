/**
 * @file router.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief This file define main router app
 * @version 0.1
 * @date 2023-10-23
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef APPS_EXAMPLE_ROUTER_H_
#define APPS_EXAMPLE_ROUTER_H_
#include <string>
#include <unordered_map>
#include <memory>

#include "core/application/application_no_ipc.h"
#include "mw/gpio_server/controller/gpio_controller.hpp"
#include "mw/i2c_service/pca9685/controller/servoController.hpp"
namespace simba {
namespace router {
class Router : public core::ApplicationNoIPC{
 protected:
  gpio::GPIOController gpio_;
  std::mutex mtx;
  core::ServoController servo_;
  /**
   * @brief This function is called to launch the application
   *
   * @param token stop token
   */
  core::ErrorCode Run(std::stop_token token) final;
  /**
   * @brief This function is called to initialiaze the application
   *
   * @param parms map with parms
   */
  core::ErrorCode Initialize(
      const std::unordered_map<std::string, std::string>& parms) final;

 public:
  ~Router() = default;
};

}  // namespace router
}  // namespace simba
#endif  // APPS_EXAMPLE_ROUTER_H_
