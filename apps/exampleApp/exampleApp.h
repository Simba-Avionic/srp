#ifndef APPS_HOMEWORK_EXAMPLEAPP_H_
#define APPS_HOMEWORK_EXAMPLEAPP_H_
#include "core/application/application_no_ipc.h"
namespace simba {
namespace example {
class exampleApp : public core::ApplicationNoIPC {
 protected:
  core::ErrorCode Run(std::stop_token token) final;
  core::ErrorCode Initialize(
      const std::unordered_map<std::string, std::string>& parms) final;
 public:
  ~exampleApp() = default;
};

}  // namespace router
}  // namespace simba
#endif  // APPS_HOMEWORK_EXAMPLEAPP_H_