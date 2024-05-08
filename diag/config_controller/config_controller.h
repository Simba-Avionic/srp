#ifndef DIAG_CONFIG_CONTROLLER_CONFIG_CONTROLLER_H_
#define DIAG_CONFIG_CONTROLLER_CONFIG_CONTROLLER_H_
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>

#include "core/common/error_code.h"
namespace simba {
namespace diag {
namespace config {
class ConfigController {
 private:
  std::unordered_map<std::string, std::string> provide{};
  std::unordered_map<std::string, std::string> require{};

 public:
  std::optional<std::string> FindProvideObject(const std::string& key) {
    const auto res = provide.find(key);
    if (res == provide.end()) {
      return {};
    }
    return res->second;
  }

  std::optional<std::string> FindRequireObject(const std::string& key) {
    const auto res = require.find(key);
    if (res == require.end()) {
      return {};
    }
    return res->second;
  }
  core::ErrorCode InsertProvideObject(const std::string& key,
                                      const std::string& val) {
    if (FindProvideObject(key).has_value()) {
      return core::ErrorCode::kError;
    }
    provide.insert({key, val});
    return core::ErrorCode::kOk;
  }
  core::ErrorCode InsertRequireObject(const std::string& key,
                                      const std::string& val) {
    if (FindRequireObject(key).has_value()) {
      return core::ErrorCode::kError;
    }
    require.insert({key, val});
    return core::ErrorCode::kOk;
  }
  ConfigController(/* args */) = default;
  ~ConfigController() = default;
};

class DiagConfig {
 private:
  static std::shared_ptr<ConfigController> controller;

 public:
  static void SetController(std::shared_ptr<ConfigController>&& con);
  static std::optional<std::string> FindProvideObject(const std::string& key);
  static std::optional<std::string> FindRequireObject(const std::string& key);
  DiagConfig(/* args */);
  ~DiagConfig();
};

}  // namespace config

}  // namespace diag
}  // namespace simba

#endif /* DIAG_CONFIG_CONTROLLER_CONFIG_CONTROLLER_H_ */
