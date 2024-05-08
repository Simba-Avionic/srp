#ifndef DIAG_CONFIG_CONTROLLER_CONFIG_CONTROLLER_JSON_H_
#define DIAG_CONFIG_CONTROLLER_CONFIG_CONTROLLER_JSON_H_

#include <memory>
#include <string>

#include "core/common/error_code.h"
#include "diag/config_controller/config_controller.h"
#include "nlohmann/json.hpp"
namespace simba {
namespace diag {
namespace config {
namespace json {
class ConfigControllerJson {
  static core::ErrorCode Validate(const nlohmann::json& obj) {
    if (!obj.contains("provide")) {
      return simba::core::ErrorCode::kError;
    }
    if (!obj.contains("require")) {
      return simba::core::ErrorCode::kError;
    }
    return core::ErrorCode::kOk;
  }

  static core::ErrorCode ParseProvide(
      const nlohmann::json& obj, std::shared_ptr<ConfigController> controller) {
    auto list = obj["provide"];
    for (auto& [key, val] : list.items()) {
      if (val.contains("id")) {
        controller->InsertProvideObject(key, val["id"]);
      }
    }
    return core::ErrorCode::kOk;
  }
  static core::ErrorCode ParseRequire(
      const nlohmann::json& obj, std::shared_ptr<ConfigController> controller) {
    auto list = obj["require"];
    for (auto& [key, val] : list.items()) {
      if (val.contains("id")) {
        controller->InsertRequireObject(key, val["id"]);
      }
    }
    return core::ErrorCode::kOk;
  }

 public:
  static core::ErrorCode ParseConfig(
      const nlohmann::json& obj, std::shared_ptr<ConfigController> controller) {
    if (Validate(obj) != core::ErrorCode::kOk) {
      return core::ErrorCode::kError;
    }
    ParseProvide(obj, controller);
    ParseRequire(obj, controller);
    return core::ErrorCode::kOk;
  }
};
}  // namespace json

}  // namespace config
}  // namespace diag
}  // namespace simba

#endif /* DIAG_CONFIG_CONTROLLER_CONFIG_CONTROLLER_JSON_H_ */
