/**
 * @file gpio_mw.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-03-08
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef MW_GPIO_SERVER_GPIO_MW_HPP_
#define MW_GPIO_SERVER_GPIO_MW_HPP_

#include "core/gpio/GPIO_digital_driver.h"
#include "communication-core/sockets/ipc_socket.h"

namespace simba {
namespace mw {

class GPIOMWService{
 private:
    com::soc::IpcSocket sock_;
    core::gpio::GpioDigitalDriver gpio_;
    void RxCallback(const std::string& ip, const std::uint16_t& port,
  const std::vector<std::uint8_t> data);
 public:

};

}
}



#endif  // MW_GPIO_SERVER_GPIO_MW_HPP_
