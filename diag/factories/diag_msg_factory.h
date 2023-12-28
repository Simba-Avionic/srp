/**
 * @file diag_msg_factory.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-12-28
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef DIAG_FACTORIES_DIAG_MSG_FACTORY_H_
#define DIAG_FACTORIES_DIAG_MSG_FACTORY_H_
#include <cstdint>
#include <vector>

#include "core/results/result.h"
#include "diag/data/data_structure.h"
namespace simba {
namespace diag {
class DiagMsgFactory {
 public:
  static core::Result<data::DataStructure> GetStructure(
      const std::vector<uint8_t>& buffer);
  static core::Result<std::vector<uint8_t>> GetBuffer(
      const data::DataStructure& data);
};
}  // namespace diag
}  // namespace simba
#endif  // DIAG_FACTORIES_DIAG_MSG_FACTORY_H_
