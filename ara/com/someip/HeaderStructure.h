/**
 * @file HeaderStructure.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-09-15
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef ARA_COM_SOMEIP_HEADERSTRUCTURE_H_
#define ARA_COM_SOMEIP_HEADERSTRUCTURE_H_
#include <bit>
#include <cstdint>
#include <vector>

#include "ara/com/com_error_domain.h"
#include "ara/com/type_converter.h"
#include "ara/core/result.h"

namespace ara {
namespace com {
namespace someip {

struct HeaderStructure {
  std::uint16_t service_id;
  std::uint16_t method_id;
  std::uint32_t length;
  std::uint16_t request_id;
  std::uint16_t session_id;
  std::uint8_t protocol_version;
  std::uint8_t interface_version;
  std::uint8_t message_type;
  std::uint8_t return_code;
};

}  // namespace someip
}  // namespace com
}  // namespace ara
namespace ara {
namespace com {

template <>
struct Convert<ara::com::someip::HeaderStructure> {
  static ara::core::Result<ara::com::someip::HeaderStructure> Conv(
      const std::vector<std::uint8_t>& in) {
    ara::com::someip::HeaderStructure res{};
    if (in.size() < 16) {
      return ara::com::MakeErrorCode(
          ara::com::ComErrc::kFieldValueIsNotValid,
          "The given buffer is of inadequate size: " +
              std::to_string(in.size()));
    }
    {
      const auto tem_v = ara::com::Convert<std::uint16_t>::Conv(
          std::vector<uint8_t>{in.begin() + 0, in.begin() + 2});
      if (!tem_v.has_value()) {
        return ara::com::MakeErrorCode(
            ara::com::ComErrc::kFieldValueIsNotValid,
            "The given buffer is of inadequate size2");
      }
      if constexpr (std::endian::native == std::endian::big) {
        res.service_id = tem_v.value();
      } else {
        res.service_id =
            ara::com::EndianConvert<std::uint16_t>::Conv(tem_v.value());
      }
    }
    {
      const auto tem_v = ara::com::Convert<std::uint16_t>::Conv(
          std::vector<uint8_t>{in.begin() + 2, in.begin() + 4});
      if (!tem_v.has_value()) {
        return ara::com::MakeErrorCode(
            ara::com::ComErrc::kFieldValueIsNotValid,
            "The given buffer is of inadequate size3");
      }
      if constexpr (std::endian::native == std::endian::big) {
        res.method_id = tem_v.value();
      } else {
        res.method_id =
            ara::com::EndianConvert<std::uint16_t>::Conv(tem_v.value());
      }
    }
    {
      const auto tem_v = ara::com::Convert<std::uint32_t>::Conv(
          std::vector<uint8_t>{in.begin() + 4, in.begin() + 8});
      if (!tem_v.has_value()) {
        return ara::com::MakeErrorCode(
            ara::com::ComErrc::kFieldValueIsNotValid,
            "The given buffer is of inadequate size4");
      }
      if constexpr (std::endian::native == std::endian::big) {
        res.length = tem_v.value();
      } else {
        res.length =
            ara::com::EndianConvert<std::uint32_t>::Conv(tem_v.value());
      }
    }
    {
      const auto tem_v = ara::com::Convert<std::uint16_t>::Conv(
          std::vector<uint8_t>{in.begin() + 8, in.begin() + 10});
      if (!tem_v.has_value()) {
        return ara::com::MakeErrorCode(
            ara::com::ComErrc::kFieldValueIsNotValid,
            "The given buffer is of inadequate size5");
      }
      if constexpr (std::endian::native == std::endian::big) {
        res.request_id = tem_v.value();
      } else {
        res.request_id =
            ara::com::EndianConvert<std::uint16_t>::Conv(tem_v.value());
      }
    }
    {
      const auto tem_v = ara::com::Convert<std::uint16_t>::Conv(
          std::vector<uint8_t>{in.begin() + 10, in.begin() + 12});
      if (!tem_v.has_value()) {
        return ara::com::MakeErrorCode(
            ara::com::ComErrc::kFieldValueIsNotValid,
            "The given buffer is of inadequate size6");
      }
      if constexpr (std::endian::native == std::endian::big) {
        res.session_id = tem_v.value();
      } else {
        res.session_id =
            ara::com::EndianConvert<std::uint16_t>::Conv(tem_v.value());
      }
    }
    {
      const auto tem_v = ara::com::Convert<std::uint8_t>::Conv(
          std::vector<uint8_t>{in.begin() + 12, in.begin() + 13});
      if (!tem_v.has_value()) {
        return ara::com::MakeErrorCode(
            ara::com::ComErrc::kFieldValueIsNotValid,
            "The given buffer is of inadequate size7");
      }
      res.protocol_version = tem_v.value();
    }
    {
      const auto tem_v = ara::com::Convert<std::uint8_t>::Conv(
          std::vector<uint8_t>{in.begin() + 13, in.begin() + 14});
      if (!tem_v.has_value()) {
        return ara::com::MakeErrorCode(
            ara::com::ComErrc::kFieldValueIsNotValid,
            "The given buffer is of inadequate size8");
      }
      res.interface_version = tem_v.value();
    }
    {
      const auto tem_v = ara::com::Convert<std::uint8_t>::Conv(
          std::vector<uint8_t>{in.begin() + 14, in.begin() + 15});
      if (!tem_v.has_value()) {
        return ara::com::MakeErrorCode(
            ara::com::ComErrc::kFieldValueIsNotValid,
            "The given buffer is of inadequate size9");
      }
      res.message_type = tem_v.value();
    }
    {
      const auto tem_v = ara::com::Convert<std::uint8_t>::Conv(
          std::vector<uint8_t>{in.begin() + 15, in.begin() + 16});
      if (!tem_v.has_value()) {
        return ara::com::MakeErrorCode(
            ara::com::ComErrc::kFieldValueIsNotValid,
            "The given buffer is of inadequate size10");
      }
      res.return_code = tem_v.value();
    }

    return res;
  }
};
template <>
struct Convert2Vector<ara::com::someip::HeaderStructure> {
  static std::vector<uint8_t> Conv(
      const ara::com::someip::HeaderStructure& in) {
    std::vector<uint8_t> out{};
    if constexpr (std::endian::native == std::endian::big) {
      const auto temp_r_v =
          ara::com::Convert2Vector<std::uint16_t>::Conv(in.service_id);
      out.insert(out.end(), temp_r_v.begin(), temp_r_v.end());
    } else {
      const auto temp_v =
          ara::com::EndianConvert<std::uint16_t>::Conv(in.service_id);
      const auto temp_r_v =
          ara::com::Convert2Vector<std::uint16_t>::Conv(temp_v);
      out.insert(out.end(), temp_r_v.begin(), temp_r_v.end());
    }
    if constexpr (std::endian::native == std::endian::big) {
      const auto temp_r_v =
          ara::com::Convert2Vector<std::uint16_t>::Conv(in.method_id);
      out.insert(out.end(), temp_r_v.begin(), temp_r_v.end());
    } else {
      const auto temp_v =
          ara::com::EndianConvert<std::uint16_t>::Conv(in.method_id);
      const auto temp_r_v =
          ara::com::Convert2Vector<std::uint16_t>::Conv(temp_v);
      out.insert(out.end(), temp_r_v.begin(), temp_r_v.end());
    }
    if constexpr (std::endian::native == std::endian::big) {
      const auto temp_r_v =
          ara::com::Convert2Vector<std::uint32_t>::Conv(in.length);
      out.insert(out.end(), temp_r_v.begin(), temp_r_v.end());
    } else {
      const auto temp_v =
          ara::com::EndianConvert<std::uint32_t>::Conv(in.length);
      const auto temp_r_v =
          ara::com::Convert2Vector<std::uint32_t>::Conv(temp_v);
      out.insert(out.end(), temp_r_v.begin(), temp_r_v.end());
    }
    if constexpr (std::endian::native == std::endian::big) {
      const auto temp_r_v =
          ara::com::Convert2Vector<std::uint16_t>::Conv(in.request_id);
      out.insert(out.end(), temp_r_v.begin(), temp_r_v.end());
    } else {
      const auto temp_v =
          ara::com::EndianConvert<std::uint16_t>::Conv(in.request_id);
      const auto temp_r_v =
          ara::com::Convert2Vector<std::uint16_t>::Conv(temp_v);
      out.insert(out.end(), temp_r_v.begin(), temp_r_v.end());
    }
    if constexpr (std::endian::native == std::endian::big) {
      const auto temp_r_v =
          ara::com::Convert2Vector<std::uint16_t>::Conv(in.session_id);
      out.insert(out.end(), temp_r_v.begin(), temp_r_v.end());
    } else {
      const auto temp_v =
          ara::com::EndianConvert<std::uint16_t>::Conv(in.session_id);
      const auto temp_r_v =
          ara::com::Convert2Vector<std::uint16_t>::Conv(temp_v);
      out.insert(out.end(), temp_r_v.begin(), temp_r_v.end());
    }
    {
      const auto temp_r_v =
          ara::com::Convert2Vector<std::uint8_t>::Conv(in.protocol_version);
      out.insert(out.end(), temp_r_v.begin(), temp_r_v.end());
    }
    {
      const auto temp_r_v =
          ara::com::Convert2Vector<std::uint8_t>::Conv(in.interface_version);
      out.insert(out.end(), temp_r_v.begin(), temp_r_v.end());
    }
    {
      const auto temp_r_v =
          ara::com::Convert2Vector<std::uint8_t>::Conv(in.message_type);
      out.insert(out.end(), temp_r_v.begin(), temp_r_v.end());
    }
    {
      const auto temp_r_v =
          ara::com::Convert2Vector<std::uint8_t>::Conv(in.return_code);
      out.insert(out.end(), temp_r_v.begin(), temp_r_v.end());
    }

    return out;
  }
};
}  // namespace com
}  // namespace ara

#endif  // ARA_COM_SOMEIP_HEADERSTRUCTURE_H_
