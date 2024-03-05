#include "TempController.h"
#include <iostream>
#include <chrono>
#include <thread>

namespace simba {
namespace mw {
namespace temp {

std::chrono::milliseconds timespan(1000);

simba::core::ErrorCode TempController::Run(std::stop_token token) {

    std::vector<uint8_t> payload{0,1,2,3};
    std::cout << "TempController is running!" << std::endl;
    while (true)
    {
        if(auto ret = this->sock_.Transmit("SIMBA.TEMP.SERVICE", 5, payload))
        {
            std::cout << "Message sent!" << std::endl;
        }
        else {
            std::cout << "Message failed!" << std::endl;
        }
        std::this_thread::sleep_for(timespan);
    }

    return core::ErrorCode::kError;
}

simba::core::ErrorCode TempController::Initialize(
      const std::unordered_map<std::string, std::string>& parms) {

      this->sock_.Init(com::soc::SocketConfig(
      "SIMBA.TEMP.SERVICE", 5, 6));
      
      std::cout << "Initialized TempController!" << std::endl;
      return simba::core::ErrorCode::kOk;
}

void TempController::Callback(const std::string& ip, const std::uint16_t& port,
    const std::vector<std::uint8_t> data) {
      
    return;
}

} // namespace temp
} // namespace mw
} // namespace simba