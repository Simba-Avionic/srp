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

#include <cstdint>
#include <string>
#include <unordered_set>
#include <vector>
#include <unordered_map>
#include <map>
#include <memory>

#include "core/gpio/gpio_driver.hpp"
#include "communication-core/sockets/stream_ipc_socket.h"
#include "ara/exec/adaptive_application.h"
#include "nlohmann/json.hpp"
#include "mw/gpio_server/gpio_mw_did.h"
#include "ara/log/log.h"

typedef uint8_t CallbackId;
typedef uint8_t PinId;

namespace srp {
namespace mw {
    constexpr std::chrono::milliseconds STATE_POLL_DELAY = std::chrono::milliseconds(100);

class GPIOMWService : public ara::exec::AdaptiveApplication {
 protected:
    std::unique_ptr<GpioMWDID> pin_did_;
    const ara::core::InstanceSpecifier did_instance;
    std::unique_ptr<srp::com::soc::ISocketStream> sock_;
    std::shared_ptr<core::gpio::IGpioDriver> gpio_driver_;
    std::unordered_map<uint8_t, GpioConf> config;

    std::unordered_map<PinId, uint8_t> subscribed_pins_states;
    std::unordered_map<PinId, std::vector<CallbackId>> callbacks;
    uint8_t next_controller_id = 1;  // indeces start from 1

    void PollSubscribedPinsLoop(const std::stop_token& token);
    std::vector<uint8_t> RxCallback(const std::string& ip, const std::uint16_t& port,
          const std::vector<std::uint8_t> data);
    std::optional<std::unordered_map<uint8_t, GpioConf>> ReadConfig(
                              const std::string& path) const;
    int InitPins();
    int Init(std::unique_ptr<srp::com::soc::ISocketStream> socket,
                              std::shared_ptr<core::gpio::IGpioDriver> gpio);

 public:
  ~GPIOMWService();
  GPIOMWService();
  int Run(const std::stop_token& token) override;

  int Initialize(const std::map<ara::core::StringView, ara::core::StringView>
                      parms) override;
};

}  // namespace mw
}  // namespace srp



#endif  // MW_GPIO_SERVER_GPIO_MW_HPP_
