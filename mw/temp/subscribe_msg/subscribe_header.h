/**
 * @file subscribe_header.h
 * @author Maciek Matuszewski (maciej.matuszewsky@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-03-02
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef MW_TEMP_SUBSCRIBE_MSG_SUBSCRIBE_HEADER_H_
#define MW_TEMP_SUBSCRIBE_MSG_SUBSCRIBE_HEADER_H_

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

 public:
  explicit SubscribeHeader(const uint16_t &service_id);
  SubscribeHeader();
  uint16_t GetServiceID() const;
  void SetData();
};

}  // namespace temp
}  // namespace mw
}  // namespace simba

#endif  // MW_TEMP_SUBSCRIBE_MSG_SUBSCRIBE_HEADER_H_
