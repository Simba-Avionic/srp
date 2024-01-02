/**
 * @file diag_data_factory.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-12-31
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef DIAG_BASE_FACTORIES_DIAG_DATA_FACTORY_H_
#define DIAG_BASE_FACTORIES_DIAG_DATA_FACTORY_H_
#include <vector>

#include "core/results/result.h"
#include "diag/base/data/data_structure.h"
namespace simba {
namespace diag {
class DiagDataFactory {
 public:
  static simba::core::Result<data::DataStructure> CreatRequest(
      const std::uint16_t service_id, const std::uint16_t diag_id,
      const std::uint16_t sender_id, const std::uint16_t transfer_id,
      const std::vector<uint8_t>& payload);
  static simba::core::Result<data::DataStructure> CreatRequestNoResponse(
      const std::uint16_t service_id, const std::uint16_t diag_id,
      const std::uint16_t sender_id, const std::uint16_t transfer_id,
      const std::vector<uint8_t>& payload);
  static simba::core::Result<data::DataStructure> CreatResponse(
      const std::uint16_t service_id, const std::uint16_t diag_id,
      const std::uint16_t sender_id, const std::uint16_t transfer_id,
      const std::vector<uint8_t>& payload);
  static simba::core::Result<data::DataStructure> CreatError(
      const std::uint16_t service_id, const std::uint16_t diag_id,
      const std::uint16_t sender_id, const std::uint16_t transfer_id,
      const std::vector<uint8_t>& payload, const uint8_t error_code);
};
}  // namespace diag
}  // namespace simba
#endif  // DIAG_BASE_FACTORIES_DIAG_DATA_FACTORY_H_
