#ifndef APPS_EC_ENGINE_SERVICE_ENGINE_APP_HPP_
#define APPS_EC_ENGINE_SERVICE_ENGINE_APP_HPP_

#include <string>
#include <unordered_map>
#include <vector>
#include <map>
#include <memory>

#include "ara/exec/adaptive_application.h"

namespace srp {
namespace apps {
class ExampleApp final : public ara::exec::AdaptiveApplication {
 protected:
  /**
   * @brief This function is called to launch the application
   *
   * @param token stop token
   */
  int Run(const std::stop_token& token) override;
  /**
   * @brief This function is called to initialiaze the application
   *
   * @param parms map with parms
   */
  int Initialize(const std::map<ara::core::StringView, ara::core::StringView>
                      parms) override;

 public:
    ExampleApp();
};

}  // namespace apps
}  // namespace srp


#endif  // APPS_EC_ENGINE_SERVICE_ENGINE_APP_HPP_
