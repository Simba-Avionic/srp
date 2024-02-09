/**
 * @file dtcService.h
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-12-23
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef MW_DIAG_DTC_SERVICE_DTCSERVICE_H_
#define MW_DIAG_DTC_SERVICE_DTCSERVICE_H_

#include <stdio.h>
#include <inttypes.h>

#include <fstream>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

#include "mw/diag/dtc/database/dtc_database.hpp"
#include "mw/diag/dtc/database/dtc_database_element.hpp"
#include "communication-core/sockets/ipc_socket.h"
#include "communication-core/sockets/socket_config.h"
#include "core/application/application_mw.h"
#include "core/application/application_factory.h"
#include "core/logger/ILogger.h"
#include "diag/dtc/factories/dtc_msg_factory.hpp"
#include "diag/base/data/data_structure.h"
#include "diag/base/data/parser.h"
#include "diag/base/factories/diag_data_factory.h"
#include "diag/base/controller/diag_controller.h"

#define MAX_STRING_LENGTH 7

namespace simba {
namespace mw {
namespace dtc {


class Converter{
 public:
  std::string uint16ToString(uint16_t value) {
    static char buffer[MAX_STRING_LENGTH];
    snprintf(buffer, sizeof(buffer), "0x%04" PRIX16, value);
    std::string result(buffer);
    return result;
}
  uint16_t convertVecToUint16(std::vector<uint8_t> data,
                                uint8_t start_index = 0) {
    uint16_t d = (static_cast<std::uint16_t>(data[start_index]) << 8) |
    static_cast<std::uint16_t>(data[start_index+1]) |
    (static_cast<std::uint16_t>(data[start_index+2]) << 8) |
    static_cast<std::uint16_t>(data[start_index+3]);
    return d;
  }
  std::string convertVecToString(std::vector<uint8_t> data,
                                        uint8_t start_index) {
    return std::string(data.begin() + start_index, data.end());
  }
  std::vector<uint8_t> convertUint16ToVector(uint16_t value) {
    std::vector<uint8_t> result;
    result.push_back(static_cast<uint8_t>((value >> 8) & 0xFF));
    result.push_back(static_cast<uint8_t>(value & 0xFF));
    return result;
  }
  std::vector<uint8_t> convertStringToVector(const std::string& str) {
    std::vector<uint8_t> result;
    for (char ch : str) {
        result.push_back(static_cast<uint8_t>(ch));
    }
    return result;
  }
};

class DtcService:public core::ApplicationMW{
 protected:
  diag::DiagDataFactory diag_factory_{};

  com::soc::IpcSocket dtc_sock_{};
  DtcDatabase db_{};

  Converter conv_{};


 public:
  DtcService();
    /**
     * @brief Callback for dtc interface
     * 
     * @param ip 
     * @param port 
     * @param data 
     */
  void DtcRxCallback(const std::string& ip, const std::uint16_t& port,
  const std::vector<std::uint8_t> data);

  /**
   * @brief This function is called to launch the application
   *
   * @param token stop token
   */
  core::ErrorCode Run(std::stop_token token) final;
  /**
   * @brief This function is called to initialize the application
   *
   * @param parms map with parms
   */
  core::ErrorCode Initialize(
      const std::unordered_map<std::string, std::string>& parms) final;
};

}  // namespace dtc
}  // namespace mw
}  // namespace simba


#endif  // MW_DIAG_DTC_SERVICE_DTCSERVICE_H_
