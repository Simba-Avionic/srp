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
#include <optional>

#include "diag/base/data/data_structure.h"
namespace simba {
namespace diag {
class DiagDataFactory {
 public:
  static std::optional<data::DataStructure> CreateReadRequest(
      const std::uint16_t service_id, const std::uint8_t diag_id,
      const std::uint16_t sender_id, const std::uint16_t transfer_id,
      const std::vector<uint8_t>& payload);
  static std::optional<data::DataStructure> CreateWriteRequest(
      const std::uint16_t service_id, const std::uint8_t diag_id,
      const std::uint16_t sender_id, const std::uint16_t transfer_id,
      const std::vector<uint8_t>& payload);
  static std::optional<data::DataStructure> CreateResponse(
      const std::uint16_t service_id, const std::uint8_t diag_id,
      const std::uint16_t sender_id, const std::uint16_t transfer_id,
      const std::vector<uint8_t>& payload);
  static std::optional<data::DataStructure> CreateJobRequest(
      const std::uint16_t service_id, const std::uint8_t diag_id,
      const std::uint16_t sender_id, const std::uint16_t transfer_id,
      const std::vector<uint8_t>& payload);
};
}  // namespace diag
}  // namespace simba
#endif  // DIAG_BASE_FACTORIES_DIAG_DATA_FACTORY_H_
