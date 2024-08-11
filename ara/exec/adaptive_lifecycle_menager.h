/**
 * @file adaptive_lifecycle_menager.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-09-01
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef ARA_EXEC_ADAPTIVE_LIFECYCLE_MENAGER_H_
#define ARA_EXEC_ADAPTIVE_LIFECYCLE_MENAGER_H_

#include <map>
#include <memory>
#include <stop_token>  // NOLINT
#include <thread>  // NOLINT
#include <utility>

#include "ara/core/string_view.h"
#include "ara/exec/adaptive_application.h"
#include "ara/log/logging_menager.h"

namespace ara {
namespace exec {
class AdaptiveLifecycleMenager final {
 private:
  std::unique_ptr<AdaptiveApplication> app_;
  std::map<ara::core::StringView, ara::core::StringView> parms_{};
  static std::shared_ptr<AdaptiveLifecycleMenager> instance_;
  std::pair<ara::core::StringView, ara::core::StringView> ParseParm(
      const ara::core::StringView &raw);
  const ara::log::Logger &exec_logger;
  std::stop_source stop_source_;
  std::unique_ptr<std::jthread> app_thread_;

 public:
  void Run(const std::stop_token &token);
  void InitApp();
  AdaptiveLifecycleMenager(int argc, char const *argv[],
                           std::unique_ptr<AdaptiveApplication> app,
                           bool controll);

  template <typename APPTYPE, typename... Params>
  static void CreateAdaptiveLifecycleMenager(int argc, char const *argv[],
                                             bool controll, Params &&...args) {
    instance_ = std::make_shared<AdaptiveLifecycleMenager>(
        argc, argv, std::make_unique<APPTYPE>(std::forward(args)...), controll);
  }

  static int StartAdaptiveLifecycleMenager();
  static void StopAdaptiveLifecycleMenager(int status_);
  ~AdaptiveLifecycleMenager();
};

}  // namespace exec
}  // namespace ara

#endif  // ARA_EXEC_ADAPTIVE_LIFECYCLE_MENAGER_H_
