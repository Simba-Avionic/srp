/**
 * @file exec_msg_factory.cpp
 * @author Wiktor Kawka (asinesak353@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-01-28
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "diag/exec/factories/exec_msg_factory.hpp"

#include <algorithm>
namespace simba {
namespace diag {
namespace exec {

std::vector<uint8_t> ExecMsgFactory::GetBuffer(
  std::shared_ptr<diag::exec::ExecHeader> header) {
  std::vector<std::uint8_t> res{header->GetBuffor()};
  return res;
}

std::shared_ptr<diag::exec::ExecHeader> ExecMsgFactory::GetHeader(
    std::vector<uint8_t> raw_data) {
  auto header = diag::exec::ExecHeader();
  header.SetBuffor(raw_data);
  return std::make_shared<diag::exec::ExecHeader>(header);
}

}  // namespace exec
}  // namespace diag
}  // namespace simba
