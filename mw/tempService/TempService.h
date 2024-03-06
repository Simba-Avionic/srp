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

#ifndef MW_TEMP_SERVICE_H_
#define MW_TEMP_SERVICE_H_

#include <string>
#include <unordered_map>
#include <stdio.h>
#include <inttypes.h>

#include <fstream>
#include <memory>
#include <vector>
#include <set>

#include "core/application/application_mw.h"
#include "communication-core/sockets/ipc_socket.h"
#include "communication-core/sockets/socket_config.h"
#include "communication-core/network-data/network_data_structure.h"
#include "core/logger/Logger.h"

#include "mw/tempService/subscribe_msg/subscribe_header.h"
#include "mw/tempService/subscribe_msg/subscribe_msg_factory.h"

namespace simba {
namespace mw {
namespace temp {

static constexpr char const* kTempServiceName = "SIMBA.TEMP.SERVICE";
static constexpr char const* kTempServiceSubName = "SIMBA.TEMP.SERVICE.SUB";

class TempApplication final : public simba::core::ApplicationMW {
 protected:
    com::soc::IpcSocket sock_{};


 private:
    std::unique_ptr<std::jthread> sub_thread;

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

  void SubCallback(const std::string& ip, const std::uint16_t& port,
    const std::vector<std::uint8_t> data);
  
  simba::core::ErrorCode Subscribe(std::stop_token stoken);
  
  void StartSubscribeThread();

};

} // namespace temp
} // namespace mw
} // namespace simba

#endif  // MW_TEMP_SERVICE_H_