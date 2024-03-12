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

#include "communication-core/sockets/ipc_socket.h"
#include "core/gpio/GPIO_digital_driver.h"
#include "mw/gpio_server/data/header.hpp"
#include "core/logger/Logger.h"

namespace simba {
namespace gpio {

enum Value{
  ERROR = -1,
  LOW = 0,
  HIGH = 1,
};

class GPIOController {
 private:
    com::soc::ISocket* sock_;
    uint16_t service_id;
 public:
    explicit GPIOController(com::soc::ISocket* socket);
    GPIOController();
    core::ErrorCode SetPinValue(uint16_t pinID, Value value);
    Value GetPinValue(uint16_t pinID);
    core::ErrorCode Init(uint16_t service_id);
};

}  // namespace gpio
}  // namespace simba


#endif  // MW_GPIO_SERVER_CONTROLLER_GPIO_CONTROLLER_HPP_
