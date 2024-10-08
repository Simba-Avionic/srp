/**
 * @file TempService.h
 * @author Maciek Matuszewski (maciej.matuszewsky@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-03-02
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef MW_TEMP_SERVICE_TEMP_SERVICE_H_
#define MW_TEMP_SERVICE_TEMP_SERVICE_H_

#include <cstdio>
#include <string>

#include <memory>
#include <vector>
#include <unordered_map>
#include <set>
#include "core/json/json_parser.h"
#include "nlohmann/json.hpp"

#include "core/application/application_mw.h"
#include "communication-core/sockets/ipc_socket.h"
#include "communication-core/sockets/socket_config.h"
#include "core/logger/Logger.h"

#include "mw/temp/subscribe_msg/subscribe_header.h"
#include "mw/temp/subscribe_msg/subscribe_msg_factory.h"

#include "mw/temp/temp_reading_msg/temp_reading_msg_factory.h"

using json = nlohmann::json;

namespace simba {
namespace mw {
namespace temp {

class TempService final : public simba::core::ApplicationMW {
 protected:
  std::unique_ptr<com::soc::IpcSocket> sub_sock_{};

 private:
  std::unique_ptr<std::jthread> temp_thread;
  std::set<std::uint16_t> subscribers{};
  std::unordered_map<std::string, std::uint8_t> sensorPathsToIds{};

  void StartTempThread();

  simba::core::ErrorCode LoadConfig(
    const std::unordered_map<std::string, std::string>& parms, std::unique_ptr<com::soc::IpcSocket> sock);
  simba::core::ErrorCode ConfigSensors() const;

  /**
   * @brief This function is called to launch the application
   *
   * @param token stop token
   */
  simba::core::ErrorCode Run(const std::stop_token& token) final;
  /**
   * @brief This function is called to initialize the application
   *
   * @param parms map with params
   */
  simba::core::ErrorCode Initialize(
      const std::unordered_map<std::string, std::string>& parms) final;

  std::vector<TempReading> RetrieveTempReadings() const;

  void SendTempReadings(const std::vector<TempReading>& readings) const;

 public:
  simba::core::ErrorCode Loop(std::stop_token stoken);

  void SubCallback(const std::string& ip, const std::uint16_t& port,
    const std::vector<std::uint8_t>& data);
};

}  // namespace temp
}  // namespace mw
}  // namespace simba

#endif  // MW_TEMP_SERVICE_TEMP_SERVICE_H_
