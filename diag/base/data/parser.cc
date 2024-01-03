/**
 * @file parser.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-12-28
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "diag/base/data/parser.h"

#include <algorithm>
namespace simba {
namespace diag {
namespace {
static constexpr size_t base_header_size = 0x08;
}
core::Result<data::DataStructure> Parser::GetStructure(
    const std::vector<uint8_t>& buffer) {
  if (buffer.size() >= base_header_size) {
    const uint16_t s_id = (buffer[0] << 8) + buffer[1];
    const uint16_t send_id = (buffer[2] << 8) + buffer[3];
    const uint8_t diag_id = buffer[4];
    const uint16_t transfer_id = (buffer[5] << 8) + buffer[6];
    data::DataStructure res{s_id, diag_id, send_id, transfer_id};
    res.SetPayload(
        std::vector<uint8_t>{buffer.begin() + base_header_size, buffer.end()});
    return core::Result{res};
  } else {
    return core::Result<data::DataStructure>{};
  }
}
core::Result<std::vector<uint8_t>> Parser::GetBuffer(
    const data::DataStructure& data) {
  std::vector<uint8_t> res{};
  res.push_back(static_cast<uint8_t>((data.GetServiceID() & 0XFF00) >> 8));
  res.push_back(static_cast<uint8_t>((data.GetServiceID() & 0X00FF)));
  res.push_back(static_cast<uint8_t>((data.GetSenderID() & 0XFF00) >> 8));
  res.push_back(static_cast<uint8_t>((data.GetSenderID() & 0X00FF)));
  res.push_back(data.GetDiagID());
  res.push_back(static_cast<uint8_t>((data.GetTransferID() & 0XFF00) >> 8));
  res.push_back(static_cast<uint8_t>((data.GetTransferID() & 0X00FF)));
  res.push_back(static_cast<uint8_t>(data.GetPayload().size() & 0xFF));
  const auto vec = data.GetPayload();
  res.insert(res.end(), vec.begin(), vec.end());
  if (res.size() < 260) {
    return core::Result{res};
  } else {
    return core::Result<std::vector<uint8_t>>{};
  }
}

}  // namespace diag
}  // namespace simba
