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

#include "communication-core/sockets/stream_ipc_socket.h"
#include "core/gpio/GPIO_digital_driver.h"
#include "mw/gpio_server/data/header.hpp"
#include "core/logger/Logger.h"

namespace simba {
namespace gpio {


class GPIOController {
 private:
    com::soc::ISocketStream* sock_;
 public:
    explicit GPIOController(com::soc::ISocketStream* socket);
    GPIOController();
    core::ErrorCode SetPinValue(uint8_t actuatorID, int8_t value);
    std::optional<int8_t> GetPinValue(uint8_t actuatorID);
};

}  // namespace gpio
}  // namespace simba


#endif  // MW_GPIO_SERVER_CONTROLLER_GPIO_CONTROLLER_HPP_
