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
#include <optional>
#include <vector>

#include "diag/base/data/data_structure.h"
namespace simba {
namespace diag {
class Parser {
 public:
  static std::optional<data::DataStructure> GetStructure(
      const std::vector<uint8_t>& buffer);
  static std::optional<std::vector<uint8_t>> GetBuffer(
      const data::DataStructure& data);
};
}  // namespace diag
}  // namespace simba
#endif  // DIAG_BASE_DATA_PARSER_H_
