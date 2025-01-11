/**
 * @file EventEntry.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-09-15
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef ARA_COM_SOMEIP_EVENTENTRY_H_
#define ARA_COM_SOMEIP_EVENTENTRY_H_
#include <bit>
#include <cstdint>
#include <vector>

#include "ara/com/com_error_domain.h"
#include "data/type_converter.h"
#include "ara/core/result.h"

namespace ara {
namespace com {
namespace someip {

struct EventEntry {
  std::uint8_t type;
  std::uint8_t index_1;
  std::uint8_t index_2;
  std::uint8_t opt;
  std::uint16_t service_id;
  std::uint16_t instance_id;
  std::uint8_t major_version;
  std::uint32_t ttl;
  std::uint16_t counter;
  std::uint16_t eventgroup_id;
};

}  // namespace someip
}  // namespace com
}  // namespace ara
namespace ara {
namespace com {

template <>
struct Convert<ara::com::someip::EventEntry> {
  static ara::core::Result<ara::com::someip::EventEntry> Conv(
      const std::vector<std::uint8_t>& in) {
    ara::com::someip::EventEntry res{};
    if (in.size() != 17) {
      return ara::com::MakeErrorCode(ara::com::ComErrc::kFieldValueIsNotValid,
                                     "The given buffer is of inadequate size");
    }
    {
      const auto tem_v = srp::data::Convert<std::uint8_t>::Conv(
          std::vector<uint8_t>{in.begin() + 0, in.begin() + 1});
      if (!tem_v.has_value()) {
        return ara::com::MakeErrorCode(
            ara::com::ComErrc::kFieldValueIsNotValid,
            "The given buffer is of inadequate size");
      }
      res.type = tem_v.value();
    }
    {
      const auto tem_v = srp::data::Convert<std::uint8_t>::Conv(
          std::vector<uint8_t>{in.begin() + 1, in.begin() + 2});
      if (!tem_v.has_value()) {
        return ara::com::MakeErrorCode(
            ara::com::ComErrc::kFieldValueIsNotValid,
            "The given buffer is of inadequate size");
      }
      res.index_1 = tem_v.value();
    }
    {
      const auto tem_v = srp::data::Convert<std::uint8_t>::Conv(
          std::vector<uint8_t>{in.begin() + 2, in.begin() + 3});
      if (!tem_v.has_value()) {
        return ara::com::MakeErrorCode(
            ara::com::ComErrc::kFieldValueIsNotValid,
            "The given buffer is of inadequate size");
      }
      res.index_2 = tem_v.value();
    }
    {
      const auto tem_v = srp::data::Convert<std::uint8_t>::Conv(
          std::vector<uint8_t>{in.begin() + 3, in.begin() + 4});
      if (!tem_v.has_value()) {
        return ara::com::MakeErrorCode(
            ara::com::ComErrc::kFieldValueIsNotValid,
            "The given buffer is of inadequate size");
      }
      res.opt = tem_v.value();
    }
    {
      const auto tem_v = srp::data::Convert<std::uint16_t>::Conv(
          std::vector<uint8_t>{in.begin() + 4, in.begin() + 6});
      if (!tem_v.has_value()) {
        return ara::com::MakeErrorCode(
            ara::com::ComErrc::kFieldValueIsNotValid,
            "The given buffer is of inadequate size");
      }
      if constexpr (std::endian::native == std::endian::big) {
        res.service_id = tem_v.value();
      } else {
        res.service_id =
            srp::data::EndianConvert<std::uint16_t>::Conv(tem_v.value());
      }
    }
    {
      const auto tem_v = srp::data::Convert<std::uint16_t>::Conv(
          std::vector<uint8_t>{in.begin() + 6, in.begin() + 8});
      if (!tem_v.has_value()) {
        return ara::com::MakeErrorCode(
            ara::com::ComErrc::kFieldValueIsNotValid,
            "The given buffer is of inadequate size");
      }
      if constexpr (std::endian::native == std::endian::big) {
        res.instance_id = tem_v.value();
      } else {
        res.instance_id =
            srp::data::EndianConvert<std::uint16_t>::Conv(tem_v.value());
      }
    }
    {
      const auto tem_v = srp::data::Convert<std::uint8_t>::Conv(
          std::vector<uint8_t>{in.begin() + 8, in.begin() + 9});
      if (!tem_v.has_value()) {
        return ara::com::MakeErrorCode(
            ara::com::ComErrc::kFieldValueIsNotValid,
            "The given buffer is of inadequate size");
      }
      res.major_version = tem_v.value();
    }
    {
      const auto tem_v = srp::data::Convert<std::uint32_t>::Conv(
          std::vector<uint8_t>{in.begin() + 9, in.begin() + 13});
      if (!tem_v.has_value()) {
        return ara::com::MakeErrorCode(
            ara::com::ComErrc::kFieldValueIsNotValid,
            "The given buffer is of inadequate size");
      }
      if constexpr (std::endian::native == std::endian::big) {
        res.ttl = tem_v.value();
      } else {
        res.ttl = srp::data::EndianConvert<std::uint32_t>::Conv(tem_v.value());
      }
    }
    {
      const auto tem_v = srp::data::Convert<std::uint16_t>::Conv(
          std::vector<uint8_t>{in.begin() + 13, in.begin() + 15});
      if (!tem_v.has_value()) {
        return ara::com::MakeErrorCode(
            ara::com::ComErrc::kFieldValueIsNotValid,
            "The given buffer is of inadequate size");
      }
      if constexpr (std::endian::native == std::endian::big) {
        res.counter = tem_v.value();
      } else {
        res.counter =
            srp::data::EndianConvert<std::uint16_t>::Conv(tem_v.value());
      }
    }
    {
      const auto tem_v = srp::data::Convert<std::uint16_t>::Conv(
          std::vector<uint8_t>{in.begin() + 15, in.begin() + 17});
      if (!tem_v.has_value()) {
        return ara::com::MakeErrorCode(
            ara::com::ComErrc::kFieldValueIsNotValid,
            "The given buffer is of inadequate size");
      }
      if constexpr (std::endian::native == std::endian::big) {
        res.eventgroup_id = tem_v.value();
      } else {
        res.eventgroup_id =
            srp::data::EndianConvert<std::uint16_t>::Conv(tem_v.value());
      }
    }

    return res;
  }
};
template <>
struct Convert2Vector<ara::com::someip::EventEntry> {
  static std::vector<uint8_t> Conv(const ara::com::someip::EventEntry& in) {
    std::vector<uint8_t> out{};
    {
      const auto temp_r_v =
          srp::data::Convert2Vector<std::uint8_t>::Conv(in.type);
      out.insert(out.end(), temp_r_v.begin(), temp_r_v.end());
    }
    {
      const auto temp_r_v =
          srp::data::Convert2Vector<std::uint8_t>::Conv(in.index_1);
      out.insert(out.end(), temp_r_v.begin(), temp_r_v.end());
    }
    {
      const auto temp_r_v =
          srp::data::Convert2Vector<std::uint8_t>::Conv(in.index_2);
      out.insert(out.end(), temp_r_v.begin(), temp_r_v.end());
    }
    {
      const auto temp_r_v =
          srp::data::Convert2Vector<std::uint8_t>::Conv(in.opt);
      out.insert(out.end(), temp_r_v.begin(), temp_r_v.end());
    }
    if constexpr (std::endian::native == std::endian::big) {
      const auto temp_r_v =
          srp::data::Convert2Vector<std::uint16_t>::Conv(in.service_id);
      out.insert(out.end(), temp_r_v.begin(), temp_r_v.end());
    } else {
      const auto temp_v =
          srp::data::EndianConvert<std::uint16_t>::Conv(in.service_id);
      const auto temp_r_v =
          srp::data::Convert2Vector<std::uint16_t>::Conv(temp_v);
      out.insert(out.end(), temp_r_v.begin(), temp_r_v.end());
    }
    if constexpr (std::endian::native == std::endian::big) {
      const auto temp_r_v =
          srp::data::Convert2Vector<std::uint16_t>::Conv(in.instance_id);
      out.insert(out.end(), temp_r_v.begin(), temp_r_v.end());
    } else {
      const auto temp_v =
          srp::data::EndianConvert<std::uint16_t>::Conv(in.instance_id);
      const auto temp_r_v =
          srp::data::Convert2Vector<std::uint16_t>::Conv(temp_v);
      out.insert(out.end(), temp_r_v.begin(), temp_r_v.end());
    }
    {
      const auto temp_r_v =
          srp::data::Convert2Vector<std::uint8_t>::Conv(in.major_version);
      out.insert(out.end(), temp_r_v.begin(), temp_r_v.end());
    }
    if constexpr (std::endian::native == std::endian::big) {
      const auto temp_r_v =
          srp::data::Convert2Vector<std::uint32_t>::Conv(in.ttl);
      out.insert(out.end(), temp_r_v.begin(), temp_r_v.end());
    } else {
      const auto temp_v = srp::data::EndianConvert<std::uint32_t>::Conv(in.ttl);
      const auto temp_r_v =
          srp::data::Convert2Vector<std::uint32_t>::Conv(temp_v);
      out.insert(out.end(), temp_r_v.begin(), temp_r_v.end());
    }
    if constexpr (std::endian::native == std::endian::big) {
      const auto temp_r_v =
          srp::data::Convert2Vector<std::uint16_t>::Conv(in.counter);
      out.insert(out.end(), temp_r_v.begin(), temp_r_v.end());
    } else {
      const auto temp_v =
          srp::data::EndianConvert<std::uint16_t>::Conv(in.counter);
      const auto temp_r_v =
          srp::data::Convert2Vector<std::uint16_t>::Conv(temp_v);
      out.insert(out.end(), temp_r_v.begin(), temp_r_v.end());
    }
    if constexpr (std::endian::native == std::endian::big) {
      const auto temp_r_v =
          srp::data::Convert2Vector<std::uint16_t>::Conv(in.eventgroup_id);
      out.insert(out.end(), temp_r_v.begin(), temp_r_v.end());
    } else {
      const auto temp_v =
          srp::data::EndianConvert<std::uint16_t>::Conv(in.eventgroup_id);
      const auto temp_r_v =
          srp::data::Convert2Vector<std::uint16_t>::Conv(temp_v);
      out.insert(out.end(), temp_r_v.begin(), temp_r_v.end());
    }

    return out;
  }
};
}  // namespace com
}  // namespace ara

#endif  // ARA_COM_SOMEIP_EVENTENTRY_H_
