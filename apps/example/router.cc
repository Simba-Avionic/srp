/**
 * @file router.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-10-23
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "apps/example/router.h"

#include "core/logger/Logger.h"
namespace simba {
namespace router {
void Router::Run(const std::unordered_map<std::string, core::Parm>& parms) {
  AppLogger::Debug("Router started");
  // this->logger_->Debug("Router started");
}
void Router::Stop() {}
}  // namespace router
}  // namespace simba
