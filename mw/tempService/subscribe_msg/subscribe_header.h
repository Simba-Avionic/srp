/**
 * @file
 * @author
 * @brief
 * @version 0.1
 * @date
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef MW_TEMP_SUBSCRIBE_HEADER_HPP_
#define MW_TEMP_SUBSCRIBE_HEADER_HPP_

#include <chrono>  // NOLINT
#include <vector>

#include "communication-core/network-data/network_data_structure.h"
#include "communication-core/network-data/network_data_type.h"

namespace simba {
namespace mw {
namespace temp {

class SubscribeHeader : public com::core::network::NetworkDataStructure {
 private:
  com::core::network::uint16_t service_id_;
  com::core::network::uint8_t length_;

 public:
  SubscribeHeader(const uint16_t &service_id);
  SubscribeHeader();
  uint16_t GetServiceID() const;
  uint8_t GetLength();
  void SetLength(const uint8_t& value);
  void SetData();

};

}  // namespace temp
}  // namespace mw
}  // namespace simba

#endif  // MW_TEMP_SUBSCRIBE_HEADER_HPP_
