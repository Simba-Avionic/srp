
#include <string>
#include <unordered_map>

#include "core/application/application_mw.h"
class TempApplication final : public simba::core::ApplicationMW {
 private:
  /**
   * @brief This function is called to launch the application
   *
   * @param token stop token
   */
  simba::core::ErrorCode Run(std::stop_token token) final;
  /**
   * @brief This function is called to initialize the application
   *
   * @param parms map with parms
   */
  simba::core::ErrorCode Initialize(
      const std::unordered_map<std::string, std::string>& parms) final;

 public:
};
