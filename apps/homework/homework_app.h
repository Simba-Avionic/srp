/**
 * @file homework_app.h
 * @author Michal Mankowski (m.mankowski2004@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-04-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef APPS_HOMEWORK_HOMEWORK_APP_H_
#define APPS_HOMEWORK_HOMEWORK_APP_H_
#include <string>
#include <unordered_map>
#include <vector>

#include "core/application/application_no_ipc.h"
#include "mw/gpio_server/controller/gpio_controller.hpp"
namespace simba {
class Homework : public core::ApplicationNoIPC{
 protected:
  gpio::GPIOController gpio_;
  core::ErrorCode Run(std::stop_token token) final;
  core::ErrorCode Initialize(
      const std::unordered_map<std::string, std::string>& parms) final;
 private:
 public:
  ~Homework() = default;
};
}  // namespace simba
#endif  // APPS_HOMEWORK_HOMEWORK_APP_H_
