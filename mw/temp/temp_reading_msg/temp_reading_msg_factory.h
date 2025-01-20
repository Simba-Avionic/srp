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

#ifndef MW_TEMP_TEMP_READING_MSG_TEMP_READING_MSG_FACTORY_H_
#define MW_TEMP_TEMP_READING_MSG_TEMP_READING_MSG_FACTORY_H_

#include <vector>
#include <memory>
#include <utility>

namespace srp {
namespace mw {
namespace temp {

// first - sensor_id, second - value
using TempReading = std::pair<uint8_t, double>;

class TempReadingMsgFactory {
 public:
  /**
   * @brief  This function return ready bit stream to send
   *
   * @param header
   * @param payload
   * @return std::vector<uint8_t>
   */
  static std::vector<uint8_t> GetBuffer(std::vector<TempReading> payload);

  /**
   * @brief Get payload from raw data
   *
   * @param raw_data
   * @return std::vector<uint8_t>
   */
  static std::vector<TempReading> GetPayload(std::vector<uint8_t> raw_data);
};

}  // namespace temp
}  // namespace mw
}  // namespace srp

#endif  // MW_TEMP_TEMP_READING_MSG_TEMP_READING_MSG_FACTORY_H_
