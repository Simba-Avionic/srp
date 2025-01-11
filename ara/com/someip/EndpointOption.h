/**
 * @file EndpointOption.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-11-26
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef ARA_COM_SOMEIP_ENDPOINTOPTION_H_
#define ARA_COM_SOMEIP_ENDPOINTOPTION_H_
#include <bit>
#include <cstdint>
#include <optional>
#include <vector>

#include "ara/com/com_error_domain.h"
#include "ara/core/result.h"
#include "data/type_converter.h"

namespace ara {
namespace com {
namespace someip {

struct EndpointOption {
  std::uint16_t length;
  std::uint8_t type;
  std::uint8_t reserved1;
  std::uint32_t ip;
  std::uint8_t reserved2;
  std::uint8_t protocol_type;
  std::uint16_t port;
};

}  // namespace someip
}  // namespace com
}  // namespace ara
namespace srp {
namespace data {

template <>
struct Convert<ara::com::someip::EndpointOption> {
  static std::optional<ara::com::someip::EndpointOption> Conv(
      const std::vector<std::uint8_t>& in) {
    ara::com::someip::EndpointOption res{};
    if (in.size() < 12) {
      return std::nullopt;
    }
    {
      const auto tem_v = srp::data::Convert<std::uint16_t>::Conv(
          std::vector<uint8_t>{in.begin() + 0, in.begin() + 2});
      if (!tem_v.has_value()) {
        return std::nullopt;
      }
      if constexpr (std::endian::native == std::endian::big) {
        res.length = tem_v.value();
      } else {
        res.length =
            srp::data::EndianConvert<std::uint16_t>::Conv(tem_v.value());
      }
    }
    {
      const auto tem_v = srp::data::Convert<std::uint8_t>::Conv(
          std::vector<uint8_t>{in.begin() + 2, in.begin() + 3});
      if (!tem_v.has_value()) {
        return std::nullopt;
      }
      res.type = tem_v.value();
    }
    {
      const auto tem_v = srp::data::Convert<std::uint8_t>::Conv(
          std::vector<uint8_t>{in.begin() + 3, in.begin() + 4});
      if (!tem_v.has_value()) {
        return std::nullopt;
      }
      res.reserved1 = tem_v.value();
    }
    {
      const auto tem_v = srp::data::Convert<std::uint32_t>::Conv(
          std::vector<uint8_t>{in.begin() + 4, in.begin() + 8});
      if (!tem_v.has_value()) {
        return std::nullopt;
      }
      if constexpr (std::endian::native == std::endian::big) {
        res.ip = tem_v.value();
      } else {
        res.ip = srp::data::EndianConvert<std::uint32_t>::Conv(tem_v.value());
      }
    }
    {
      const auto tem_v = srp::data::Convert<std::uint8_t>::Conv(
          std::vector<uint8_t>{in.begin() + 8, in.begin() + 9});
      if (!tem_v.has_value()) {
        return std::nullopt;
      }
      res.reserved2 = tem_v.value();
    }
    {
      const auto tem_v = srp::data::Convert<std::uint8_t>::Conv(
          std::vector<uint8_t>{in.begin() + 9, in.begin() + 10});
      if (!tem_v.has_value()) {
        return std::nullopt;
      }
      res.protocol_type = tem_v.value();
    }
    {
      const auto tem_v = srp::data::Convert<std::uint16_t>::Conv(
          std::vector<uint8_t>{in.begin() + 10, in.begin() + 12});
      if (!tem_v.has_value()) {
        return std::nullopt;
      }
      if constexpr (std::endian::native == std::endian::big) {
        res.port = tem_v.value();
      } else {
        res.port = srp::data::EndianConvert<std::uint16_t>::Conv(tem_v.value());
      }
    }

    return res;
  }
};
template <>
struct Convert2Vector<ara::com::someip::EndpointOption> {
  static std::vector<uint8_t> Conv(const ara::com::someip::EndpointOption& in) {
    std::vector<uint8_t> out{};
    if constexpr (std::endian::native == std::endian::big) {
      const auto temp_r_v =
          srp::data::Convert2Vector<std::uint16_t>::Conv(in.length);
      out.insert(out.end(), temp_r_v.begin(), temp_r_v.end());
    } else {
      const auto temp_v =
          srp::data::EndianConvert<std::uint16_t>::Conv(in.length);
      const auto temp_r_v =
          srp::data::Convert2Vector<std::uint16_t>::Conv(temp_v);
      out.insert(out.end(), temp_r_v.begin(), temp_r_v.end());
    }
    {
      const auto temp_r_v =
          srp::data::Convert2Vector<std::uint8_t>::Conv(in.type);
      out.insert(out.end(), temp_r_v.begin(), temp_r_v.end());
    }
    {
      const auto temp_r_v =
          srp::data::Convert2Vector<std::uint8_t>::Conv(in.reserved1);
      out.insert(out.end(), temp_r_v.begin(), temp_r_v.end());
    }
    if constexpr (std::endian::native == std::endian::big) {
      const auto temp_r_v =
          srp::data::Convert2Vector<std::uint32_t>::Conv(in.ip);
      out.insert(out.end(), temp_r_v.begin(), temp_r_v.end());
    } else {
      const auto temp_v = srp::data::EndianConvert<std::uint32_t>::Conv(in.ip);
      const auto temp_r_v =
          srp::data::Convert2Vector<std::uint32_t>::Conv(temp_v);
      out.insert(out.end(), temp_r_v.begin(), temp_r_v.end());
    }
    {
      const auto temp_r_v =
          srp::data::Convert2Vector<std::uint8_t>::Conv(in.reserved2);
      out.insert(out.end(), temp_r_v.begin(), temp_r_v.end());
    }
    {
      const auto temp_r_v =
          srp::data::Convert2Vector<std::uint8_t>::Conv(in.protocol_type);
      out.insert(out.end(), temp_r_v.begin(), temp_r_v.end());
    }
    if constexpr (std::endian::native == std::endian::big) {
      const auto temp_r_v =
          srp::data::Convert2Vector<std::uint16_t>::Conv(in.port);
      out.insert(out.end(), temp_r_v.begin(), temp_r_v.end());
    } else {
      const auto temp_v = srp::data::EndianConvert<std::uint16_t>::Conv(in.port);
      const auto temp_r_v =
          srp::data::Convert2Vector<std::uint16_t>::Conv(temp_v);
      out.insert(out.end(), temp_r_v.begin(), temp_r_v.end());
    }

    return out;
  }
};
}  // namespace data
}  // namespace srp

#endif  // ARA_COM_SOMEIP_ENDPOINTOPTION_H_
