/**
 * @file dtc_header_factory.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-12-23
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef MW_DIAG_DTC_MSG_DTC_MSG_FACTORY_HPP_
#define MW_DIAG_DTC_MSG_DTC_MSG_FACTORY_HPP_

#include <memory>
#include <vector>

#include "mw/diag/dtc/msg/dtc_header.hpp"

namespace simba {
namespace mw {
namespace dtc {

class DtcMsgFactory {
 public:
  /**
   * @brief  This function return ready bit stream to send
   *
   * @param header
   * @param payload
   * @return std::vector<uint8_t>
   */
  std::vector<uint8_t> GetBuffer(std::shared_ptr<DtcHeader> header,
                                 std::vector<uint8_t> payload);

  /**
   * @brief Creat header object from raw data
   *
   * @param raw_data
   * @return std::shared_ptr<DtcHeader>
   */
  std::shared_ptr<DtcHeader> GetHeader(std::vector<uint8_t> raw_data);

  /**
   * @brief Get payload from raw data
   *
   * @param raw_data
   * @return std::vector<uint8_t>
   */
  std::vector<uint8_t> GetPayload(std::vector<uint8_t> raw_data);
};

}  // namespace dtc
}  // namespace mw
}  // namespace simba

#endif  // MW_DIAG_DTC_MSG_DTC_MSG_FACTORY_HPP_
