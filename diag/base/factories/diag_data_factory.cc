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
std::optional<data::DataStructure> DiagDataFactory::CreateReadRequest(
    const std::uint16_t service_id, const std::uint8_t diag_id,
    const std::uint16_t sender_id, const std::uint16_t transfer_id,
    const std::vector<uint8_t>& payload) {
  if (diag_id > 0x3F) {
    return {};
  }
  data::DataStructure res{service_id,
                          static_cast<uint8_t>(((diag_id & 0x3F) << 0x02) |
                                               static_cast<uint8_t>(0x01U)),
                          sender_id, transfer_id};
  res.SetPayload(payload);

  return res;
}

std::optional<data::DataStructure> DiagDataFactory::CreateWriteRequest(
    const std::uint16_t service_id, const std::uint8_t diag_id,
    const std::uint16_t sender_id, const std::uint16_t transfer_id,
    const std::vector<uint8_t>& payload) {
  if (diag_id > 0x3F) {
    return {};
  }
  data::DataStructure res{service_id,
                          static_cast<uint8_t>(((diag_id & 0x3F) << 0x02) |
                                               static_cast<uint8_t>(0x00U)),
                          sender_id, transfer_id};
  res.SetPayload(payload);

  return res;
}

std::optional<data::DataStructure> DiagDataFactory::CreateResponse(
    const std::uint16_t service_id, const std::uint8_t diag_id,
    const std::uint16_t sender_id, const std::uint16_t transfer_id,
    const std::vector<uint8_t>& payload) {
  if (diag_id > 0x3F) {
    return {};
  }
  data::DataStructure res{service_id,
                          static_cast<uint8_t>(((diag_id & 0x3F) << 0x02) |
                                               static_cast<uint8_t>(0x03U)),
                          sender_id, transfer_id};
  res.SetPayload(payload);

  return res;
}

std::optional<data::DataStructure> DiagDataFactory::CreateJobRequest(
    const std::uint16_t service_id, const std::uint8_t diag_id,
    const std::uint16_t sender_id, const std::uint16_t transfer_id,
    const std::vector<uint8_t>& payload) {
  if (diag_id > 0x3F) {
    return {};
  }

  data::DataStructure res{service_id,
                          static_cast<uint8_t>(((diag_id & 0x3F) << 0x02) |
                                               static_cast<uint8_t>(0x02U)),
                          sender_id, transfer_id};
  res.SetPayload(payload);

  return res;
}
}  // namespace diag
}  // namespace simba
