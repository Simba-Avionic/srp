/**
 * @file data_structure.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-12-28
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "diag/base/data/data_structure.h"

#include <algorithm>
namespace simba {
namespace diag {
namespace data {

DataStructure::DataStructure(const std::uint16_t service_id,
                             const std::uint8_t diag_id,
                             const std::uint16_t sender_id,
                             const std::uint16_t transfer_id)
    : service_id_{service_id},
      sender_id_{sender_id},
      diag_id_{diag_id},
      transfer_id_{transfer_id} {}
void DataStructure::SetPayload(const std::vector<uint8_t>& payload) {
  std::copy(payload.begin(), payload.end(), std::back_inserter(payload_));
}
std::vector<uint8_t> DataStructure::GetPayload() const { return payload_; }
const std::uint16_t DataStructure::GetServiceID() const { return service_id_; }
const std::uint8_t DataStructure::GetDiagID() const { return diag_id_; }
const std::uint16_t DataStructure::GetSenderID() const { return sender_id_; }
const std::uint16_t DataStructure::GetTransferID() const {
  return transfer_id_;
}

const bool DataStructure::isWrite() const { return (diag_id_ & 0x03) == 0x00; }
const bool DataStructure::isRead() const { return (diag_id_ & 0x03) == 0x01; }
const bool DataStructure::isJob() const { return (diag_id_ & 0x03) == 0x02; }
const bool DataStructure::isResponse() const {
  return (diag_id_ & 0x03) == 0x03;
}

}  // namespace data
}  // namespace diag
}  // namespace simba
