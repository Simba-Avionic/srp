/**
 * @file diag_msg_factory.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-12-28
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "diag/factories/diag_msg_factory.h"

#include <algorithm>
namespace simba {
namespace diag {
core::Result<data::DataStructure> DiagMsgFactory::GetStructure(
    const std::vector<uint8_t>& buffer) {
  if (buffer.size() >= 0x04) {
    const uint16_t s_id = (buffer[0] << 8) + buffer[1];
    data::DataStructure res{s_id, buffer[2]};
    res.SetPayload(std::vector<uint8_t>{buffer.begin() + 0x04, buffer.end()});
    return core::Result{res};
  } else {
    return core::Result<data::DataStructure>{};
  }
}
core::Result<std::vector<uint8_t>> DiagMsgFactory::GetBuffer(
    const data::DataStructure& data) {
  std::vector<uint8_t> res{};
  res.push_back(static_cast<uint8_t>((data.GetServiceID() & 0XFF00) >> 8));
  res.push_back(static_cast<uint8_t>((data.GetServiceID() & 0X00FF)));
  res.push_back(data.GetDiagID());
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
