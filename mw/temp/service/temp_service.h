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

#include <string>
#include <cstdio>
#include <cinttypes>
#include <cstdint>
#include <cstring>
#include <sys/types.h>  // NOLINT
#include <sys/socket.h>  // NOLINT
#include <netinet/in.h>  // NOLINT

#include <fstream>
#include <memory>
#include <vector>
#include <unordered_map>
#include <set>
#include <future>  // NOLINT
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

static constexpr char const*
  kTempServiceName = "SIMBA.TEMP.SERVICE";

static constexpr char const*
  kSubscriberPrefix = "SIMBA.TEMP.";

class TempService final : public simba::core::ApplicationMW {
 protected:
  com::soc::IpcSocket sub_sock_{};
  const std::chrono::milliseconds temp_timeout{750};

 private:
  std::unique_ptr<std::jthread> temp_thread;
  std::set<std::uint16_t> subscribers{};
  std::unordered_map<std::string, std::uint8_t> sensorPathsToIds{};
  simba::mw::temp::SubMsgFactory factory;

  void StartTempThread();

  simba::core::ErrorCode LoadConfig(
    const std::unordered_map<std::string, std::string>& parms);
  simba::core::ErrorCode ConfigSensors();

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

  std::vector<TempReading> RetrieveTempReadings();

  void SendTempReadings(const std::vector<TempReading>& readings);

 public:
  simba::core::ErrorCode Loop(std::stop_token stoken);

  void SubCallback(const std::string& ip, const std::uint16_t& port,
    const std::vector<std::uint8_t> data);
};

}  // namespace temp
}  // namespace mw
}  // namespace simba

#endif  // MW_TEMP_SERVICE_TEMP_SERVICE_H_
