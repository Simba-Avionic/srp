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

#ifndef MW_TEMP_SUBSCRIBE_MSG_FACTORY_HPP_
#define MW_TEMP_SUBSCRIBE_MSG_FACTORY_HPP_

#include "subscribe_header.h"
#include <vector>
#include <memory>

namespace simba {
namespace mw {
namespace temp {

class SubMsgFactory {
 public:
  /**
   * @brief  This function return ready bit stream to send
   *
   * @param header
   * @param payload
   * @return std::vector<uint8_t>
   */
  std::vector<uint8_t> GetBuffer(std::shared_ptr<simba::mw::temp::SubscribeHeader> header,
                                 std::vector<uint8_t> payload);

  /**
   * @brief Creat header object from raw data
   *
   * @param raw_data
   * @return std::shared_ptr<SubscribeHeader>
   */
  std::shared_ptr<simba::mw::temp::SubscribeHeader> GetHeader(
    std::vector<uint8_t> raw_data);

  /**
   * @brief Get payload from raw data
   *
   * @param raw_data
   * @return std::vector<uint8_t>
   */
  std::vector<uint8_t> GetPayload(std::vector<uint8_t> raw_data);
};

}  // namespace temp
}  // namespace mw
}  // namespace simba

#endif  // MW_TEMP_SUBSCRIBE_MSG_FACTORY_HPP_
