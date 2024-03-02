#include "TempService.h"
#include <iostream>

namespace simba {
namespace mw {
namespace temp {

simba::core::ErrorCode TempApplication::Run(std::stop_token token) {
      this->sock_.Init(com::soc::SocketConfig(
      "SIMBA.TEMP.SERVICE", 5, 5));

      std::cout << "dziala?" << std::endl;
      this->sock_.SetRXCallback(
            std::bind(&simba::mw::temp::TempApplication::TempCallback, this, 
                  std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
      this->sock_.StartRXThread();
      this->SleepMainThread();
      return core::ErrorCode::kOk;
}

simba::core::ErrorCode TempApplication::Initialize(
      const std::unordered_map<std::string, std::string>& parms) {
      
      return simba::core::ErrorCode::kOk;
}

void TempApplication::TempCallback(const std::string& ip, const std::uint16_t& port,
      const std::vector<std::uint8_t> data) {
      
      return;
}

} // namespace temp
} // namespace mw
} // namespace simba