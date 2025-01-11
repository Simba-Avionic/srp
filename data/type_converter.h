/**
 * @file type_converter.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-09-15
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef DATA_TYPE_CONVERTER_H_
#define DATA_TYPE_CONVERTER_H_

#include <algorithm>
#include <array>
#include <bit>  // NOLINT
#include <cstdint>
#include <cstring>
#include <optional>
#include <vector>

namespace srp {
namespace data {
template <typename T_OUT>
struct Convert {
  static std::optional<T_OUT> Conv(const std::vector<std::uint8_t>& in) {
    if (in.size() < sizeof(uint8_t)) {
      return std::nullopt;
    }
    return in.at(0);
  }
};

template <>
struct Convert<uint8_t> {
  static std::optional<uint8_t> Conv(const std::vector<std::uint8_t>& in) {
    if (in.size() < sizeof(uint8_t)) {
      return std::nullopt;
    }
    return in.at(0);
  }
};
template <>
struct Convert<int8_t> {
  static std::optional<int8_t> Conv(const std::vector<std::uint8_t>& in) {
    if (in.size() < sizeof(int8_t)) {
      return std::nullopt;
    }
    return static_cast<int8_t>(in.at(0));
  }
};

template <std::size_t size, typename T_OUT>
T_OUT ConverVector2Number(const std::vector<uint8_t>& in) {
  T_OUT temp;
  std::memcpy(&temp, in.data(), size);
  return temp;
}

template <>
struct Convert<uint16_t> {
  static std::optional<uint16_t> Conv(const std::vector<std::uint8_t>& in) {
    if (in.size() < sizeof(uint16_t)) {
      return std::nullopt;
    }
    return ConverVector2Number<sizeof(uint16_t), uint16_t>(in);
  }
};
template <>
struct Convert<uint32_t> {
  static std::optional<uint32_t> Conv(const std::vector<std::uint8_t>& in) {
    if (in.size() < sizeof(uint32_t)) {
      return std::nullopt;
    }
    return ConverVector2Number<sizeof(uint32_t), uint32_t>(in);
  }
};
template <>
struct Convert<uint64_t> {
  static std::optional<uint64_t> Conv(const std::vector<std::uint8_t>& in) {
    if (in.size() < sizeof(uint64_t)) {
      return std::nullopt;
    }
    return ConverVector2Number<sizeof(uint64_t), uint64_t>(in);
  }
};

template <>
struct Convert<int16_t> {
  static std::optional<int16_t> Conv(const std::vector<std::uint8_t>& in) {
    if (in.size() < sizeof(int16_t)) {
      return std::nullopt;
    }
    return ConverVector2Number<sizeof(int16_t), int16_t>(in);
  }
};
template <>
struct Convert<int32_t> {
  static std::optional<int32_t> Conv(const std::vector<std::uint8_t>& in) {
    if (in.size() < sizeof(int32_t)) {
      return std::nullopt;
    }
    return ConverVector2Number<sizeof(int32_t), int32_t>(in);
  }
};
template <>
struct Convert<int64_t> {
  static std::optional<int64_t> Conv(const std::vector<std::uint8_t>& in) {
    if (in.size() < sizeof(int64_t)) {
      return std::nullopt;
    }
    return ConverVector2Number<sizeof(int64_t), int64_t>(in);
  }
};

template <>
struct Convert<float> {
  static std::optional<float> Conv(const std::vector<std::uint8_t>& in) {
    if (in.size() < sizeof(float)) {
      return std::nullopt;
    }
    return ConverVector2Number<sizeof(float), float>(in);
  }
};
template <>
struct Convert<double> {
  static std::optional<double> Conv(const std::vector<std::uint8_t>& in) {
    if (in.size() < sizeof(double)) {
      return std::nullopt;
    }
    return ConverVector2Number<sizeof(double), double>(in);
  }
};

////////////////////// Conv TO VECTOR //////////////////////////////////

template <typename T_IN>
struct Convert2Vector {
  static std::vector<uint8_t> Conv(const T_IN& in) {
    return std::vector<uint8_t>{static_cast<uint8_t>(in)};
  }
};

template <>
struct Convert2Vector<bool> {
  static std::vector<uint8_t> Conv(const bool& in) {
    if (in) {
      return std::vector<uint8_t>{1U};
    }
    return std::vector<uint8_t>{0U};
  }
};

template <>
struct Convert2Vector<uint8_t> {
  static std::vector<uint8_t> Conv(const uint8_t& in) {
    return std::vector<uint8_t>{in};
  }
};
template <>
struct Convert2Vector<int8_t> {
  static std::vector<uint8_t> Conv(const int8_t& in) {
    return std::vector<uint8_t>{static_cast<uint8_t>(in)};
  }
};

template <std::size_t size, typename T_IN>
std::vector<uint8_t> ConvNumber2Vector(const T_IN& in) {
  std::array<uint8_t, size> help_buf;
  std::memcpy(&help_buf, &in, size);
  return std::vector<uint8_t>{help_buf.begin(), help_buf.end()};
}

template <>
struct Convert2Vector<uint16_t> {
  static std::vector<uint8_t> Conv(const uint16_t& in) {
    return ConvNumber2Vector<sizeof(in)>(in);
  }
};
template <>
struct Convert2Vector<uint32_t> {
  static std::vector<uint8_t> Conv(const uint32_t& in) {
    return ConvNumber2Vector<sizeof(in)>(in);
  }
};
template <>
struct Convert2Vector<uint64_t> {
  static std::vector<uint8_t> Conv(const uint64_t& in) {
    return ConvNumber2Vector<sizeof(in)>(in);
  }
};

template <>
struct Convert2Vector<int16_t> {
  static std::vector<uint8_t> Conv(const int16_t& in) {
    return ConvNumber2Vector<sizeof(in)>(in);
  }
};
template <>
struct Convert2Vector<int32_t> {
  static std::vector<uint8_t> Conv(const int32_t& in) {
    return ConvNumber2Vector<sizeof(in)>(in);
  }
};
template <>
struct Convert2Vector<int64_t> {
  static std::vector<uint8_t> Conv(const int64_t& in) {
    return ConvNumber2Vector<sizeof(in)>(in);
  }
};

template <>
struct Convert2Vector<float> {
  static std::vector<uint8_t> Conv(const float& in) {
    return ConvNumber2Vector<sizeof(in)>(in);
  }
};
template <>
struct Convert2Vector<double> {
  static std::vector<uint8_t> Conv(const double& in) {
    return ConvNumber2Vector<sizeof(in)>(in);
  }
};
////////////////////// Conv Endings //////////////////////////////////
template <typename T>
struct EndianConvert {
  static T Conv(T in, std::endian req_type) {
    if (std::endian::native == req_type) {
      return in;
    } else {
      std::array<std::uint8_t, sizeof(T)> buffor;
      std::memcpy(&buffor, &in, sizeof(T));
      std::reverse(buffor.begin(), buffor.end());
      std::memcpy(&in, &buffor, sizeof(T));
      return in;
    }
  }
  static T Conv(T in) {
    std::array<std::uint8_t, sizeof(T)> buffor;
    std::memcpy(&buffor, &in, sizeof(T));
    std::reverse(buffor.begin(), buffor.end());
    std::memcpy(&in, &buffor, sizeof(T));
    return in;
  }
};

}  // namespace data
}  // namespace srp
#endif  // DATA_TYPE_CONVERTER_H_
