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
#include <map>
#include <memory>

#include "core/gpio/gpio_driver.hpp"
#include "communication-core/sockets/stream_ipc_socket.h"
#include "ara/exec/adaptive_application.h"
#include "nlohmann/json.hpp"
#include "mw/gpio_server/gpio_mw_did.h"

namespace simba {
namespace mw {

class GPIOMWService : public ara::exec::AdaptiveApplication {
 protected:
  std::unique_ptr<GpioMWDID> pin_did_;
    std::unique_ptr<com::soc::ISocketStream> sock_;
    std::shared_ptr<core::gpio::IGpioDriver> gpio_driver_;
    std::unordered_map<uint8_t, GpioConf> config;
    std::vector<uint8_t> RxCallback(const std::string& ip, const std::uint16_t& port,
          const std::vector<std::uint8_t> data);
    std::optional<std::unordered_map<uint8_t, GpioConf>> ReadConfig(
                              std::string path) const;
    int InitPins();
    int Init(std::unique_ptr<com::soc::ISocketStream> socket,
                              std::shared_ptr<core::gpio::IGpioDriver> gpio);

 public:
  ~GPIOMWService();
  int Run(const std::stop_token& token) override;

  int Initialize(const std::map<ara::core::StringView, ara::core::StringView>
                      parms) override;
};

}  // namespace mw
}  // namespace simba



#endif  // MW_GPIO_SERVER_GPIO_MW_HPP_
