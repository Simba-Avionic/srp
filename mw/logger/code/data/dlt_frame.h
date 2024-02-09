/**
 * @file dlt_frame.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-02-07
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef MW_LOGGER_CODE_DATA_DLT_FRAME_H_
#define MW_LOGGER_CODE_DATA_DLT_FRAME_H_
#include <algorithm>
#include <string>
#include <vector>

#include "core/results/result.h"
#include "mw/logger/code/data/dlt_log_type.h"
#include "mw/logger/code/data/idlt_frame.h"
namespace simba {
namespace dlt {
namespace data {
template <typename T>
class DltFrame final : public IDLTFrame {
 private:
  /**
   * @brief Header type
   *
   */
  const uint8_t HTYP{0x27};
  /**
   * @brief Message Counter
   *
   */
  uint8_t MCNT{0x01};
  /**
   * @brief Length
   *
   */
  uint16_t LEN{0x12};
  /**
   * @brief ECU ID
   *
   */
  std::string ECU{};
  /**
   * @brief Message Info
   *
   */
  uint8_t MSIN{0x00};
  /**
   * @brief Number of Arguments
   *
   */
  const uint8_t NOAR{0x01};
  /**
   * @brief Application ID
   *
   */
  std::string APID{""};
  /**
   * @brief Context ID (NOT supported now!)
   *
   */
  std::string CTID{"NONE"};
  const T payload{"test"};

 public:
  DltFrame(const std::string& ecu_id, const std::string& app_id,
           const DLTLogType type, const T& payload_)
      : payload{payload_} {
    ECU = ecu_id;
    APID = app_id;
    MSIN = type;
  }
  core::Result<std::vector<uint8_t>> ParseFrame() noexcept override {
    LEN += payload.Length();
    std::vector<uint8_t> res{HTYP, MCNT, static_cast<uint8_t>(LEN >> 8U),
                             static_cast<uint8_t>(LEN & 0xFFU)};
    std::copy(this->ECU.begin(), this->ECU.end(), std::back_inserter(res));
    res.push_back(MSIN);
    res.push_back(NOAR);
    std::copy(this->APID.begin(), this->APID.end(), std::back_inserter(res));
    std::copy(this->CTID.begin(), this->CTID.end(), std::back_inserter(res));
    const auto r = payload.ParseArg();
    std::copy(r.begin(), r.end(), std::back_inserter(res));
    return core::Result{res};
  }
};
}  // namespace data
}  // namespace dlt
}  // namespace simba
#endif  // MW_LOGGER_CODE_DATA_DLT_FRAME_H_
