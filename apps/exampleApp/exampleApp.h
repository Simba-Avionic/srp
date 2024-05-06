/**
 * @file exampleApp.h
 * @author Jacek Kukawski (jacekka6@gmail.com)
 * @brief 
 * @version 1.0
 * @date 2024-05-06
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef APPS_HOMEWORK_EXAMPLEAPP_H_
#define APPS_HOMEWORK_EXAMPLEAPP_H_
#include <unordered_map>
#include <string>
//#include <vector>

#include "core/application/application_no_ipc.h"
#include "mw/gpio_server/controller/gpio_controller.hpp"
#include "core/logger/Logger.h"

namespace simba {
namespace example {
class exampleApp : public core::ApplicationNoIPC {
 protected:
  core::ErrorCode Run(std::stop_token token) final;
  core::ErrorCode Initialize(
      const std::unordered_map<std::string, std::string>& parms) final;
  gpio::GPIOController gpio_controller_;
 public:
  ~exampleApp() = default;
};
}  // namespace router
}  // namespace simba
#endif  // APPS_HOMEWORK_EXAMPLEAPP_H_