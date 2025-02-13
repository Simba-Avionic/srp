/**
 * @file gpio_controller.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-03-08
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef MW_GPIO_SERVER_CONTROLLER_GPIO_CONTROLLER_HPP_
#define MW_GPIO_SERVER_CONTROLLER_GPIO_CONTROLLER_HPP_

#include <string>
#include <vector>
#include <unordered_map>
#include <optional>
#include <memory>

#include "communication-core/sockets/stream_ipc_socket.h"
#include "core/gpio/gpio_driver.hpp"
#include "mw/gpio_server/data/header.hpp"
#include "mw/gpio_server/controller/Igpio_controller.h"

namespace srp {
namespace gpio {


class GPIOController : public IGPIOController{
 private:
    std::unique_ptr<srp::com::soc::ISocketStream> sock_;
    std::optional<srp::com::soc::RXCallback> callback = std::nullopt;
    std::unordered_set<uint8_t> subsbribed_pins;
    void HandleCallback(const std::vector<std::uint8_t> data);
    void ListenToCallbacks();
 public:
    ~GPIOController();
    GPIOController(): sock_(std::make_unique<com::soc::StreamIpcSocket>()) {}
    explicit GPIOController(std::unique_ptr<srp::com::soc::ISocketStream> socket);
    core::ErrorCode SetPinValue(uint8_t actuatorID, int8_t value) override;
    std::optional<int8_t> GetPinValue(uint8_t actuatorID) override;

    core::ErrorCode SetCallback(const srp::com::soc::RXCallback callback);
    core::ErrorCode SubscribePin(const uint8_t pin_id, bool subscribe);
};

}  // namespace gpio
}  // namespace srp


#endif  // MW_GPIO_SERVER_CONTROLLER_GPIO_CONTROLLER_HPP_
