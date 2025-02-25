/**
 * @file network_data_structure.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief This file define class for creat network data structure
 * @version 1.0
 * @date 2023-08-25
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef COMMUNICATION_CORE_NETWORK_DATA_NETWORK_DATA_STRUCTURE_H_
#define COMMUNICATION_CORE_NETWORK_DATA_NETWORK_DATA_STRUCTURE_H_
#include <cstdint>
#include <vector>

#include "communication-core/network-data/iframe.h"
namespace srp {
namespace com {
namespace core {
namespace network {
class NetworkDataStructure : public interface::IFrame {
 private:
  std::vector<interface::IFrame*> structure_;

 protected:
  void AddData(interface::IFrame* field);

 public:
  NetworkDataStructure(/* args */) = default;
  // NetworkDataStructure(const NetworkDataStructure&) = delete;
  virtual ~NetworkDataStructure() = default;

  /**
   * @brief Return Structure as uint8 vector
   *
   * @return std::vector<std::uint8_t>
   */
  std::vector<std::uint8_t> GetBuffor() const override;

  srp::core::ErrorCode SetBuffor(std::vector<std::uint8_t> data) override;
};
}  // namespace network
}  // namespace core
}  // namespace com
}  // namespace srp
#endif  // COMMUNICATION_CORE_NETWORK_DATA_NETWORK_DATA_STRUCTURE_H_
