/**
 * @file parser.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-12-28
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef DIAG_BASE_DATA_PARSER_H_
#define DIAG_BASE_DATA_PARSER_H_
#include <cstdint>
#include <vector>

#include "core/results/result.h"
#include "diag/base/data/data_structure.h"
namespace simba {
namespace diag {
class Parser {
 public:
  static core::Result<data::DataStructure> GetStructure(
      const std::vector<uint8_t>& buffer);
  static core::Result<std::vector<uint8_t>> GetBuffer(
      const data::DataStructure& data);
};
}  // namespace diag
}  // namespace simba
#endif  // DIAG_BASE_DATA_PARSER_H_
