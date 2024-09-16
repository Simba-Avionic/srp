/**
 * @file someip_frame.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-09-15
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef ARA_COM_SOMEIP_SOMEIP_FRAME_H_
#define ARA_COM_SOMEIP_SOMEIP_FRAME_H_
#include <vector>

#include "ara/com/someip/HeaderStructure.h"
#include "ara/com/type_converter.h"
#include "ara/core/result.h"
namespace ara {
namespace com {
namespace someip {
class SomeipFrame {
 public:
  HeaderStructure header_;
  const std::vector<uint8_t> payload_;

 private:
  SomeipFrame(const HeaderStructure& header,
              const std::vector<uint8_t>& payload);
  explicit SomeipFrame(const HeaderStructure& header);

 public:
  template <typename T>
  ara::core::Result<T> GetPayload() const {
    return ara::com::Convert<T>::Conv(payload_);
  }
  std::vector<uint8_t> GetRaw() const;
  static ara::core::Result<SomeipFrame> MakeFrame(
      const std::vector<uint8_t>& raw_frame);
  static SomeipFrame MakeFrame(HeaderStructure& header,  // NOLINT
                               const std::vector<uint8_t>& payload);
  static SomeipFrame MakeFrame(HeaderStructure& header);  // NOLINT

  static SomeipFrame MakeResponseFrame(const SomeipFrame& header,
                                       const std::vector<uint8_t>& payload);
  ~SomeipFrame() = default;
};
}  // namespace someip
}  // namespace com
}  // namespace ara

#endif  // ARA_COM_SOMEIP_SOMEIP_FRAME_H_
