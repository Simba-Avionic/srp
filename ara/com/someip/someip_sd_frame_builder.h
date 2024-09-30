/**
 * @file someip_sd_frame_builder.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef ARA_COM_SOMEIP_SOMEIP_SD_FRAME_BUILDER_H_
#define ARA_COM_SOMEIP_SOMEIP_SD_FRAME_BUILDER_H_

#include <vector>

#include "ara/com/someip/HeaderStructure.h"
#include "ara/com/someip/ServiceEntry.h"
#include "ara/com/someip/EndpointOption.h"

#include "ara/com/someip/message_code.h"
#include "ara/com/someip/message_type.h"
#include "ara/com/someip/someip_frame.h"

namespace ara {
namespace com {
namespace someip {
class SomeipSdFrameBuilder {
 protected:
  HeaderStructure header_{};
  uint8_t flag{0xc0};
  std::vector<ServiceEntry> entry_list_{};
  std::vector<EndpointOption> endpoint_list_{};
  void AddEntry(ServiceEntry&& entry_);
    void AddEntry(ServiceEntry&& entry_, EndpointOption&& endpoint);

 public:
  SomeipSdFrameBuilder(/* args */);

  SomeipSdFrameBuilder& SetRequestId(const uint16_t& id);
  SomeipSdFrameBuilder& SetSessionId(const uint16_t& id);
  SomeipSdFrameBuilder& AddFindEntry(const uint16_t& service_id,
                                     const uint16_t& instace_id,
                                     const uint8_t major_version,
                                     const uint32_t& minor_version);
  SomeipSdFrameBuilder& AddOfferEntry(const uint16_t& service_id,
                                      const uint16_t& instace_id,
                                      const uint8_t major_version,
                                      const uint32_t& minor_version,
                                      const uint32_t ip, const uint16_t port);
  uint8_t GetSize() const;
  SomeipFrame BuildFrame() noexcept;
  virtual ~SomeipSdFrameBuilder() = default;
};
}  // namespace someip
}  // namespace com
}  // namespace ara
#endif  // ARA_COM_SOMEIP_SOMEIP_SD_FRAME_BUILDER_H_
