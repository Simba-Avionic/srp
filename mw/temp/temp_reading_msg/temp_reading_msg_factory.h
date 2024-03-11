/**
 * @file temp_reading_msg_factory.h
 * @author Maciek Matuszewski (maciej.matuszewsky@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-03-02
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef MW_TEMPSERVICE_TEMP_READING_MSG_TEMP_READING_MSG_FACTORY_H_
#define MW_TEMPSERVICE_TEMP_READING_MSG_TEMP_READING_MSG_FACTORY_H_

#include "temp_reading_msg.h"
#include <vector>
#include <memory>

namespace simba {
namespace mw {
namespace temp {

class TempReadingMsgFactory {
 public:
  /**
   * @brief  This function return ready bit stream to send
   *
   * @param header
   * @param payload
   * @return std::vector<uint8_t>
   */
  std::vector<uint8_t> GetBuffer(std::shared_ptr<simba::mw::temp::TempReadingMsg> header,
                                 std::vector<TempReading> payload);

  /**
   * @brief Create header object from raw data
   *
   * @param raw_data
   * @return std::shared_ptr<SubscribeHeader>
   */
  std::shared_ptr<simba::mw::temp::TempReadingMsg> GetHeader(
    std::vector<uint8_t> raw_data);

  /**
   * @brief Get payload from raw data
   *
   * @param raw_data
   * @return std::vector<uint8_t>
   */
  std::vector<TempReading> GetPayload(std::vector<uint8_t> raw_data);
};

}  // namespace temp
}  // namespace mw
}  // namespace simba

#endif  // MW_TEMPSERVICE_TEMP_READING_MSG_TEMP_READING_MSG_FACTORY_H_
