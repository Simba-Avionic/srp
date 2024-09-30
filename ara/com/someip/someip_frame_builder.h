/**
 * @file someip_frame_builder.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef ARA_COM_SOMEIP_SOMEIP_FRAME_BUILDER_H_
#define ARA_COM_SOMEIP_SOMEIP_FRAME_BUILDER_H_

#include <vector>

#include "ara/com/someip/HeaderStructure.h"
#include "ara/com/someip/message_code.h"
#include "ara/com/someip/message_type.h"
#include "ara/com/someip/someip_frame.h"
namespace ara {
namespace com {
namespace someip {
class SomeipFrameBuilder {
 protected:
  HeaderStructure header_{};
  std::vector<uint8_t> payload_{};

 public:
  SomeipFrameBuilder(/* args */);

  SomeipFrameBuilder& SetServiceId(const uint16_t& id);
  SomeipFrameBuilder& SetMethodId(const uint16_t& id);
  SomeipFrameBuilder& SetRequestId(const uint16_t& id);
  SomeipFrameBuilder& SetSessionId(const uint16_t& id);
  SomeipFrameBuilder& SetMessageType(const MessageType& type);
  SomeipFrameBuilder& SetReturnCode(const MessageCode& code);
  SomeipFrameBuilder& SetPayload(const std::vector<uint8_t>&& payload);
  SomeipFrameBuilder& CopyFrameData(const HeaderStructure& header);
  SomeipFrame BuildFrame() noexcept;
  virtual ~SomeipFrameBuilder() = default;
};
}  // namespace someip
}  // namespace com
}  // namespace ara
#endif  // ARA_COM_SOMEIP_SOMEIP_FRAME_BUILDER_H_
