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
#include "communication-core/network-data/network_data_type.h"

namespace simba {
namespace diag {
namespace exec {

class ExecHeader : public com::core::network::NetworkDataStructure {
 private:
  // numer id serwisu
  com::core::network::uint16_t service_id_;
  // timestamp
  com::core::network::uint32_t time_stamp_;
  com::core::network::uint8_t flags_;
 public:
  ExecHeader(const uint16_t &service_id,
  const uint16_t &time_stamp, uint8_t flags);
  ExecHeader();
  void SetData() {
    this->AddData(&service_id_);
    this->AddData(&time_stamp_);
    this->AddData(&flags_);
  }


  uint16_t GetServiceID() const;
  uint16_t GetTimestamp() const;
  uint8_t GetFlags() const;
  void IncrementTimeStamp();
};

}  // namespace exec
}  // namespace diag
}  // namespace simba

#endif  // DIAG_EXEC_DATA_EXEC_HEADER_HPP_
