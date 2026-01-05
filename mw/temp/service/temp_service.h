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
#include <map>
#include <unordered_map>
#include <set>
#include "core/json/json_parser.h"
#include "nlohmann/json.hpp"

#include "communication-core/sockets/stream_ipc_socket.h"
#include "communication-core/sockets/socket_config.h"
#include "ara/log/log.h"
#include "ara/exec/adaptive_application.h"
#include "srp/mw/temp/TempReadHdr.h"
#include "srp/mw/temp/TempSubHdr.h"
#include "core/temp/temp_driver.hpp"
#include "mw/temp/service/temp_mw_did.h"

using json = nlohmann::json;

namespace srp {
namespace mw {
namespace temp {

class TempService final : public ara::exec::AdaptiveApplication {
 protected:
  std::unique_ptr<com::soc::StreamIpcSocket> sub_sock_{};

 private:
  std::unique_ptr<core::temp::ITempDriver> temp_driver_;
  std::unique_ptr<TempMWDID> temp_did_;
  const ara::core::InstanceSpecifier did_instance;
  std::unordered_map<uint8_t, std::set<uint8_t>> subscribers{};
  uint16_t delay_time;
  uint8_t nextSensorId = 0;
  //                 physical ID, sensor ID
  std::unordered_map<std::string, std::uint8_t> sensorPathsToIds{};


  int ConfigSensors();

  /**
   * @brief This function is called to launch the application
   *
   * @param token stop token
   */
  int Run(const std::stop_token& token) override;
  /**
   * @brief This function is called to initialize the application
   *
   * @param parms map with params
   */
  int Initialize(const std::map<ara::core::StringView, ara::core::StringView>
                      parms) override;

  std::vector<srp::mw::temp::TempReadHdr> RetrieveTempReadings() const;

  void SendTempReadings(const std::vector<srp::mw::temp::TempReadHdr>& readings);
  std::vector<uint8_t> Conv(const std::vector<srp::mw::temp::TempReadHdr>& readings) const;

 public:
  TempService();
  int Loop(std::stop_token stoken);

  std::vector<uint8_t> SubCallback(const std::string& ip, const std::uint16_t& port,
    const std::vector<std::uint8_t>& data);
};

}  // namespace temp
}  // namespace mw
}  // namespace srp

#endif  // MW_TEMP_SERVICE_TEMP_SERVICE_H_
