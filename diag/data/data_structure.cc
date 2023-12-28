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
#include "diag/data/data_structure.h"

#include <algorithm>
namespace simba {
namespace diag {
namespace data {

DataStructure::DataStructure(const std::uint16_t service_id,
                             const std::uint8_t diag_id)
    : service_id_{service_id}, diag_id_{diag_id} {}
void DataStructure::SetPayload(const std::vector<uint8_t>& payload) {
  std::copy(payload.begin(), payload.end(), std::back_inserter(payload_));
}
std::vector<uint8_t> DataStructure::GetPayload() const { return payload_; }
const std::uint16_t DataStructure::GetServiceID() const { return service_id_; }
const std::uint8_t DataStructure::GetDiagID() const { return diag_id_; }
}  // namespace data
}  // namespace diag
}  // namespace simba
