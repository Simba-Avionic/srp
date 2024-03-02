/**
 * @file TempSerivce.h
 * @author Maciek Matuszewski (maciej.matuszewsky@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-03-02
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef MW_TEMP_CONTROLLER_H_
#define MW_TEMP_CONTROLLER_H_

#include <string>
#include <unordered_map>
#include <stdio.h>
#include <inttypes.h>

#include <fstream>
#include <memory>
#include <vector>

#include "core/application/application_mw.h"
#include "communication-core/sockets/ipc_socket.h"
#include "communication-core/sockets/socket_config.h"

namespace simba {
namespace mw {
namespace temp {

class TempController final : public simba::core::ApplicationMW {
 protected:
    com::soc::IpcSocket sock_{};

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
   * @param parms map with params
   */
  simba::core::ErrorCode Initialize(
      const std::unordered_map<std::string, std::string>& parms) final;


 public:
};

} // namespace temp
} // namespace mw
} // namespace simba

#endif  // MW_TEMP_SERVICE_H_