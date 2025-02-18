/**
 * @file example_did.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-09-01
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef MW_TEMP_SERVICE_TEMP_MW_DID_H_
#define MW_TEMP_SERVICE_TEMP_MW_DID_H_
#include <string>
#include <vector>
#include <memory>
#include <sstream>
#include <cstring>
#include <unordered_map>
#include <bitset>
#include "ara/log/log.h"
#include "ara/diag/generic_data_identifier.h"
#include "ara/diag/uds_error_domain.h"
namespace srp {
namespace mw {

static std::vector<uint8_t> floatToVector(float value) {
    std::vector<uint8_t> vec(sizeof(float));
    std::memcpy(vec.data(), &value, sizeof(float));
    return vec;
}

static std::vector<uint8_t> mapToVector(const std::unordered_map<uint8_t, float>& data) {
    std::vector<uint8_t> result;
    for (const auto& [key, value] : data) {
        result.push_back(key);
        std::vector<uint8_t> floatBytes = floatToVector(value);
        result.insert(result.end(), floatBytes.begin(), floatBytes.end());
    }
    return result;
}

class TempMWDID : public ara::diag::GenericDiD {
 private:
  std::unordered_map<uint8_t, float> data;
  /**
   * @brief Callback for 0x22 UDS job (Read data by id) *optional
   *
   * @return DiagResponse
   */
  ara::core::Result<ara::diag::OperationOutput> Read() noexcept override {
    return ara::diag::OperationOutput{mapToVector(data)};
  }
  ara::core::Result<void> Write(
    const std::vector<uint8_t> &payload) noexcept override {
    return ara::diag::MakeErrorCode(
        ara::diag::UdsDiagErrc::kSubFunctionNotSupported);
}

 public:
  void UpdateTemp(uint8_t sensor_id, float temp) {
    auto it = data.find(sensor_id);
    if (it == data.end()) {
      return;
    }
    it->second = temp;
  }
  TempMWDID(const ara::core::InstanceSpecifier &specifier,
          std::vector<uint8_t> sensors): ara::diag::GenericDiD{specifier} {
    for (const auto& sensor : sensors) {
      data[sensor] = 0.0;
    }
    }
};

}  // namespace mw
}  // namespace srp

#endif  // MW_TEMP_SERVICE_TEMP_MW_DID_H_
