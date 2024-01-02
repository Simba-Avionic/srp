/**
 * @file data_structure.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief This
 * @version 0.1
 * @date 2023-12-28
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef DIAG_BASE_DATA_DATA_STRUCTURE_H_
#define DIAG_BASE_DATA_DATA_STRUCTURE_H_
#include <vector>

#include "cstdint"
namespace simba {
namespace diag {
namespace data {
/**
 * @brief This class hold all data for diagnostic communication
 *
 */
class DataStructure {
 private:
  /**
   * @brief Service id in diagnostic
   *
   */
  const std::uint16_t service_id_;
  /**
   * @brief Sender id in diagnostic
   *
   */
  const std::uint16_t sender_id_;
  /**
   * @brief Diagnostic method ID
   *
   */
  const std::uint16_t diag_id_;
  /**
   * @brief Transfer id
   *
   */
  const std::uint16_t transfer_id_;
  /**
   * @brief Vector with payload
   *
   */
  std::vector<uint8_t> payload_{};

 public:
  DataStructure(const std::uint16_t service_id, const std::uint16_t diag_id,
                const std::uint16_t sender_id, const std::uint16_t transfer_id);
  void SetPayload(const std::vector<uint8_t>& payload);
  std::vector<uint8_t> GetPayload() const;
  const std::uint16_t GetServiceID() const;
  const std::uint16_t GetSenderID() const;
  const std::uint16_t GetDiagID() const;
  const std::uint16_t GetTransferID() const;
};
}  // namespace data
}  // namespace diag
}  // namespace simba
#endif  // DIAG_BASE_DATA_DATA_STRUCTURE_H_
