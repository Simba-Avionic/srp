/**
 * @file exec_header.hpp
 * @author Wiktor Kawka (asinesak353@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-01-27
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef DIAG_EXEC_DATA_EXEC_HEADER_HPP_
#define DIAG_EXEC_DATA_EXEC_HEADER_HPP_

#include <chrono>  // NOLINT
#include <vector>

#include "communication-core/network-data/network_data_structure.h"

namespace simba {
namespace diag {
namespace exec {

class ExecHeader : public com::core::network::NetworkDataStructure {
 private:
  // numer id serwisu
  const uint16_t service_id_;
  // timestamp
  uint16_t time_stamp_;
  uint8_t flags_;
 public:
  ExecHeader(const uint16_t &service_id,
  const uint16_t &time_stamp, uint8_t flags);
  ExecHeader();

  uint16_t GetServiceID() const;
  uint16_t GetTimestamp() const;
  uint8_t GetFlags() const;
  void IncrementTimeStamp();
};

}  // namespace exec
}  // namespace diag
}  // namespace simba

#endif  // DIAG_EXEC_DATA_EXEC_HEADER_HPP_
