/**
 * @file diag_data_factory.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-12-31
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "diag/base/factories/diag_data_factory.h"

#include <cassert>
namespace simba {
namespace diag {
simba::core::Result<data::DataStructure> DiagDataFactory::CreatRequest(
    const std::uint16_t service_id, const std::uint16_t diag_id,
    const std::uint16_t sender_id, const std::uint16_t transfer_id,
    const std::vector<uint8_t>& payload) {
  if (diag_id > 0xFFF) {
    return simba::core::Result<data::DataStructure>{};
  }
  data::DataStructure res{service_id, static_cast<uint16_t>(((diag_id & 0xFFF) << 0x04) | static_cast<uint16_t>(0x00U)),
                          sender_id, transfer_id};
  res.SetPayload(payload);

  return simba::core::Result{res};
}

simba::core::Result<data::DataStructure>
DiagDataFactory::CreatRequestNoResponse(const std::uint16_t service_id,
                                        const std::uint16_t diag_id,
                                        const std::uint16_t sender_id,
                                        const std::uint16_t transfer_id,
                                        const std::vector<uint8_t>& payload) {
  if (diag_id > 0xFFF) {
    return simba::core::Result<data::DataStructure>{};
  }
  data::DataStructure res{service_id, static_cast<uint16_t>(((diag_id & 0xFFF) << 0x04) | static_cast<uint16_t>(0x02U)),
                          sender_id, transfer_id};
  res.SetPayload(payload);

  return simba::core::Result{res};
}

simba::core::Result<data::DataStructure> DiagDataFactory::CreatResponse(
    const std::uint16_t service_id, const std::uint16_t diag_id,
    const std::uint16_t sender_id, const std::uint16_t transfer_id,
    const std::vector<uint8_t>& payload) {
  if (diag_id > 0xFFF) {
    return simba::core::Result<data::DataStructure>{};
  }
  data::DataStructure res{service_id, static_cast<uint16_t>(((diag_id & 0xFFF) << 0x04) | static_cast<uint16_t>(0x01U)),
                          sender_id, transfer_id};
  res.SetPayload(payload);

  return simba::core::Result{res};
}

simba::core::Result<data::DataStructure> DiagDataFactory::CreatError(
    const std::uint16_t service_id, const std::uint16_t diag_id,
    const std::uint16_t sender_id, const std::uint16_t transfer_id,
    const std::vector<uint8_t>& payload, const uint8_t error_code) {
  if (diag_id > 0xFFF) {
    return simba::core::Result<data::DataStructure>{};
  }
  if (error_code < 0x3 || error_code > 0xf) {
    return simba::core::Result<data::DataStructure>{};
  }

  data::DataStructure res{service_id, static_cast<uint16_t>(((diag_id & 0xFFF) << 0x04) | static_cast<uint16_t>(0x01U)),
                          sender_id, transfer_id};
  res.SetPayload(payload);

  return simba::core::Result{res};
}
}  // namespace diag
}  // namespace simba
