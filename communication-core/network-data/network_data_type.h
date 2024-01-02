/**
 * @file network_data_type.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief This file define network data types template class
 * @version 1.0
 * @date 2023-08-25
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef COMMUNICATION_CORE_NETWORK_DATA_NETWORK_DATA_TYPE_H_
#define COMMUNICATION_CORE_NETWORK_DATA_NETWORK_DATA_TYPE_H_
#include <algorithm>
#include <array>
#include <cstring>
#include <utility>
#include <vector>

#include "core/common/endianess_converter.h"
#include "communication-core/network-data/iframe.h"
namespace simba {
namespace com {
namespace core {
namespace network {
template <typename T, size_t size>
class NetworkDataType : public interface::IFrame {
 protected:
  T value_;

 public:
  uint8_t GetSize() const override { return size; }
  NetworkDataType(/* args */) : interface::IFrame(size) {}
  explicit NetworkDataType(T value) : value_{value} {}
  virtual ~NetworkDataType() = default;
  NetworkDataType<T, size> operator=(const T& other) {
    this->value_ = other;
    return *this;
  }
  void SetFromNetwork(T value) {
    this->value_ = ::simba::core::converter::EndianessConverter::Convert(value);
  }
  void Set(const T& value) { this->value_ = value; }
  T Get() const { return this->value_; }
  T GetForNetwork() {
    return simba::core::converter::EndianessConverter::Convert(this->value_);
  }
  __attribute_noinline__ std::vector<std::uint8_t> GetBuffor() const override {
    T value_temp =
        simba::core::converter::EndianessConverter::Convert(this->value_);
    std::array<uint8_t, size> help_buf;
    std::memcpy(&help_buf, &value_temp, size);
    std::vector<std::uint8_t> buffor{help_buf.begin(), help_buf.end()};
    return buffor;
  }
  simba::core::ErrorCode SetBuffor(std::vector<std::uint8_t> data) override {
    std::array<uint8_t, size> help_array;
    if (data.size() != size) {
      return simba::core::kBadVariableSize;
    }
    std::copy(data.begin(), data.end(), help_array.begin());
    std::memcpy(&value_, &help_array, size);
    value_ = simba::core::converter::EndianessConverter::Convert(this->value_);
    return simba::core::kOk;
  }
};

/**
 * @brief int8_t for network big endianess
 *
 */
using int8_t = NetworkDataType<int8_t, 1>;
/**
 * @brief int16_t for network big endianess
 *
 */
using int16_t = NetworkDataType<std::int16_t, 2>;
/**
 * @brief int32_t for network big endianess
 *
 */
using int32_t = NetworkDataType<std::int32_t, 4>;
/**
 * @brief int64_t for network big endianess
 *
 */
using int64_t = NetworkDataType<std::int64_t, 8>;
/**
 * @brief uint8_t for network big endianess
 *
 */
using uint8_t = NetworkDataType<std::uint8_t, 1>;
/**
 * @brief uint16_t for network big endianess
 *
 */
using uint16_t = NetworkDataType<std::uint16_t, 2>;
/**
 * @brief uint32_t for network big endianess
 *
 */
using uint32_t = NetworkDataType<std::uint32_t, 4>;
/**
 * @brief uint64_t for network big endianess
 *
 */
using uint64_t = NetworkDataType<std::uint64_t, 8>;
/**
 * @brief float for network big endianess
 *
 */
using float_t = NetworkDataType<float, sizeof(float)>;
/**
 * @brief double for network big endianess
 *
 */
using double_t = NetworkDataType<double, sizeof(double)>;

using bool_t = NetworkDataType<bool, 1>;
}  // namespace network
}  // namespace core
}  // namespace com
}  // namespace simba
#endif  // COMMUNICATION_CORE_NETWORK_DATA_NETWORK_DATA_TYPE_H_
