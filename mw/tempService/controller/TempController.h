/**
 * @file TempController.h
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
#include <unordered_map>
#include <set>
#include <future>
#include "nlohmann/json.hpp"

#include "core/application/application_mw.h"
#include "communication-core/sockets/ipc_socket.h"
#include "communication-core/sockets/socket_config.h"
#include "core/logger/Logger.h"

#include "mw/tempService/subscribe_msg/subscribe_header.h"
#include "mw/tempService/subscribe_msg/subscribe_msg_factory.h"

using json = nlohmann::json;

namespace simba {
namespace mw {
namespace temp {

static constexpr char const* 
  kTempControllerName = "SIMBA.TEMP.CONTROLLER";

static const std::string kTempConfigPath = 
  "mw/tempService/controller/temp_config.json";

struct TempReading
{
  uint16_t sensor_id;
  _Float64 value;
};

class TempController final : public simba::core::ApplicationMW {
 protected:
  com::soc::IpcSocket sub_sock_{};
  const std::chrono::milliseconds temp_timeout{750};

 private:
  std::unique_ptr<std::jthread> temp_thread;
  std::set<std::uint16_t> subscribers{};
  std::unordered_map<std::string, std::uint8_t> sensorPathsToIds{};
  simba::mw::temp::SubMsgFactory factory;

  void StartTempThread();

  simba::core::ErrorCode LoadConfig(const std::string& path);

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

  // TempReading GetTempReading(std::string& path);

  void RetrieveTempReadings(std::vector<TempReading> &readings, 
    std::vector<std::future<simba::core::ErrorCode>>& futures);
  void SendTempReadings(std::vector<TempReading> &readings, 
    std::vector<std::future<simba::core::ErrorCode>>& futures);

 public:

  simba::core::ErrorCode Loop(std::stop_token stoken);

  void SubCallback(const std::string& ip, const std::uint16_t& port,
    const std::vector<std::uint8_t> data);

};

} // namespace temp
} // namespace mw
} // namespace simba

#endif  // MW_TEMP_CONTROLLER_H_


// diag::dtc::DtcMsgFactory factory;
// auto hdr = factory.GetHeader(data);
// std::vector<uint8_t> payload = factory.GetPayload(data);
// this->db_.AddError(hdr->GetDtcID(),
//                    this->conv_.convertVecToString(payload, 0));
// AppLogger::Debug("Zarejestrowano błąd " +
//                  std::to_string(static_cast<int>(hdr->GetDtcID())));
// this->diag_controller->Write(
//     0x0201, 0x0001, this->conv_.convertUint16ToVector(hdr->GetDtcID()));