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

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

#include "core/gpio/GPIO_digital_driver.h"
#include "communication-core/sockets/stream_ipc_socket.h"
#include "core/application/application_mw.h"
#include "nlohmann/json.hpp"

namespace simba {
namespace mw {

struct GpioConf{
  uint16_t pinNum;
  core::gpio::direction_t direction;
};

class GPIOMWService : public simba::core::ApplicationMW {
 protected:
    std::unique_ptr<com::soc::ISocketStream> sock_;
    std::unique_ptr<core::gpio::IgpioDigitalDriver> gpio_driver_;
    std::unordered_map<uint8_t, GpioConf> config;
    std::vector<uint8_t> RxCallback(const std::string& ip, const std::uint16_t& port,
          const std::vector<std::uint8_t> data);
    std::optional<std::unordered_map<uint8_t, GpioConf>> ReadConfig(nlohmann::json data);
    core::ErrorCode InitPins();
    core::ErrorCode Init(std::unique_ptr<com::soc::ISocketStream> socket,
                              std::unique_ptr<core::gpio::IgpioDigitalDriver> gpio);
 public:
  core::ErrorCode Run(std::stop_token token) final;

  core::ErrorCode Initialize(
      const std::unordered_map<std::string, std::string>& parms) final;
};

}  // namespace mw
}  // namespace simba



#endif  // MW_GPIO_SERVER_GPIO_MW_HPP_
