#include "mw/diag/dtc/service/lib/converter.h"

#include <algorithm>

#include "communication-core/network-data/network_data_type.h"

namespace simba {
namespace mw {
namespace dtc {
namespace converter {
template <>
std::vector<uint8_t> VectorConverter::Convert(const std::vector<uint32_t>& base,
                                              const bool invert) {
  std::vector<uint8_t> res{};
  for (const auto& ele : base) {
    std::array<uint8_t, 4> help_buf;
    std::memcpy(help_buf.data(), &ele, 4);
    if (!invert) {
      std::copy(help_buf.cend(), help_buf.cbegin() + 1,
                std::back_inserter(res));
    } else {
      std::copy(help_buf.cbegin() + 1, help_buf.cend(),
                std::back_inserter(res));
    }
  }
  return std::move(res);
}

template <>
std::vector<uint8_t> VectorConverter::Convert(const uint32_t& base,
                                              const bool invert) {
  std::array<uint8_t, sizeof(uint32_t)> temp{};
  std::memcpy(temp.data(), &base, sizeof(uint32_t));
  std::vector<uint8_t> res{};
  if (!invert) {
    std::copy(temp.cbegin() + 1, temp.cend(), std::back_inserter(res));
  } else {
    std::reverse_copy(temp.cbegin(), temp.cend()-1, std::back_inserter(res));
  }
  return res;
}

template <>
std::vector<uint8_t> VectorConverter::Convert(
    const std::vector<uint32_t>& base) {
  return VectorConverter::Convert(base, false);
}
template <>
std::vector<uint8_t> VectorConverter::Convert(const uint32_t& base) {
  return VectorConverter::Convert(base, false);
}
}  // namespace converter
}  // namespace dtc
}  // namespace mw
}  // namespace simba