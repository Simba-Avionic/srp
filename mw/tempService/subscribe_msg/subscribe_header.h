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

namespace simba {
namespace mw {
namespace temp {

class SubscribeHeader : public com::core::network::NetworkDataStructure {
 private:
  const uint16_t service_id_;

 public:
  SubscribeHeader(const uint16_t &service_id);
  SubscribeHeader();
  uint16_t GetServiceID() const;
};

}  // namespace temp
}  // namespace mw
}  // namespace simba

#endif  // MW_TEMP_SUBSCRIBE_HEADER_HPP_
