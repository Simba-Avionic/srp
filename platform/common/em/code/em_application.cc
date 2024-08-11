/**
 * @file em_application.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-02-06
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "platform/common/em/code/em_application.h"

#include <map>
#include <string>

#include "core/common/condition.h"
namespace simba {
namespace em {
EmApplication::EmApplication(/* args */) {}

EmApplication::~EmApplication() {}

int EmApplication::Run(const std::stop_token& token) {
  this->em_service.StartApps();
  core::condition::wait(token);
  return 0;
}
/**
 * @brief This function is called to initialize the application
 *
 * @param parms map with parms
 */
int EmApplication::Initialize(
    const std::map<ara::core::StringView, ara::core::StringView> parms) {
  this->em_service.LoadApps();
  return 0;
}

}  // namespace em
}  // namespace simba
