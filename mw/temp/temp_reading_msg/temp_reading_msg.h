/**
 * @file temp_reading_msg.h
 * @author Maciek Matuszewski (maciej.matuszewsky@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-03-02
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef MW_TEMPSERVICE_TEMP_READING_MSG_TEMP_READING_MSG_H_
#define MW_TEMPSERVICE_TEMP_READING_MSG_TEMP_READING_MSG_H_

#include <chrono>  // NOLINT
#include <utility>
#include <vector>

#include "communication-core/network-data/network_data_structure.h"
#include "communication-core/network-data/network_data_type.h"

namespace simba {
namespace mw {
namespace temp {

// first: sensor_id, second: value
using TempReading = std::pair<uint16_t, double>;

class TempReadingMsg : public com::core::network::NetworkDataStructure {
 public:
  TempReadingMsg();
};

}  // namespace temp
}  // namespace mw
}  // namespace simba

#endif  // MW_TEMPSERVICE_TEMP_READING_MSG_TEMP_READING_MSG_H_
