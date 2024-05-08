/**
 * @file config_controller.cc
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-05-08
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "diag/config_controller/config_controller.h"

#include <memory>
namespace simba {
namespace diag {
namespace config {
std::shared_ptr<ConfigController> DiagConfig::controller = nullptr;

void DiagConfig::SetController(std::shared_ptr<ConfigController>&& con) {
  controller = std::move(con);
}
std::optional<std::string> DiagConfig::FindProvideObject(
    const std::string& key) {
  if (controller) {
    return controller->FindProvideObject(std::move(key));
  } else {
    return {};
  }
}
std::optional<std::string> DiagConfig::FindRequireObject(
    const std::string& key) {
  if (controller) {
    return controller->FindRequireObject(std::move(key));
  } else {
    return {};
  }
}
}  // namespace config
}  // namespace diag
}  // namespace simba