/**
 * @file callbacks.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-03-24
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef COMMUNICATION_CORE_SOMEIP_CONTROLLER_CALLBACKS_H_
#define COMMUNICATION_CORE_SOMEIP_CONTROLLER_CALLBACKS_H_
#include <cstdint>
#include <functional>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "communication-core/someip-database/code/endpoint.h"
#include "communication-core/someip-database/code/interface.h"
#include "communication-core/someip/message_code.h"
#include "communication-core/someip/message_type.h"

namespace simba {
namespace com {
namespace someip {

using ResultCallback =
    std::function<void(const std::vector<uint8_t> payload,
                       data::MessageCode code, const uint16_t transfer_id)>;

using SendCallback = std::function<uint16_t(
    const std::vector<uint8_t> payload, const objects::Endpoint& endpoint,
    const objects::Interface& interface, data::MessageType type,
    ResultCallback result)>;
using MethodCallback = std::function<
    std::pair<data::MessageCode, std::optional<std::vector<uint8_t>>>(
        const std::vector<uint8_t> payload, const objects::Endpoint endpoint)>;

using LocalMethodCallback = std::function<std::optional<std::vector<uint8_t>>(
    const std::vector<uint8_t> payload)>;

using DropTransferCallback = std::function<void(const uint16_t transfer_id)>;

using SubscribeCallback = std::function<void(const objects::Endpoint endpoint,
                                             const objects::Interface interface,
                                             ResultCallback result)>;
using FindServiceCallbackResult =
    std::function<void(const objects::Interface& interface)>;
using FindCallback = std::function<void(
    const objects::Endpoint& endpoint, FindServiceCallbackResult res_callback)>;
}  // namespace someip
}  // namespace com
}  // namespace simba

#endif  // COMMUNICATION_CORE_SOMEIP_CONTROLLER_CALLBACKS_H_
