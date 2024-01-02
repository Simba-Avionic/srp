/**
 * @file network_data_structure.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief This file define class for creat network data structure
 * @version 1.0
 * @date 2023-08-25
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "communication-core/network-data/network_data_structure.h"

#include <cstdint>
#include <iostream>
#include <utility>
#include <vector>
namespace simba {
namespace com {
namespace core {
namespace network {

void NetworkDataStructure::AddData(interface::IFrame* field) {
  this->structure_.push_back(field);
  this->SetSize(this->GetSize() + field->GetSize());
}

std::vector<std::uint8_t> NetworkDataStructure::GetBuffor() const {
  std::vector<std::uint8_t> buffor{};
  for (auto field : this->structure_) {
    auto temp = field->GetBuffor();
    buffor.insert(buffor.end(), temp.begin(), temp.end());
  }
  return buffor;
}

simba::core::ErrorCode NetworkDataStructure::SetBuffor(
    std::vector<std::uint8_t> data) {
  uint16_t offset = 0;
  simba::core::ErrorCode res = simba::core::ErrorCode::kOk;
  for (auto field : this->structure_) {
    if (data.begin() + offset + field->GetSize() <= data.end()) {
      std::vector<std::uint8_t> temp{};
      temp.insert(temp.end(), data.begin() + offset,
                  data.begin() + offset + field->GetSize());
      field->SetBuffor(temp);
      offset += field->GetSize();
    } else {
      res = simba::core::ErrorCode::kBadVariableSize;
      std::cerr << "error for field data size:" << data.size() << std::endl;
    }
  }
  return res;
}

}  // namespace network
}  // namespace core
}  // namespace com
}  // namespace simba
