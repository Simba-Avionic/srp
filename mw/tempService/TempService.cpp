#include "TempService.h"
#include <iostream>

namespace simba {
namespace mw {
namespace temp {


simba::core::ErrorCode TempApplication::Run(std::stop_token token) {

      this->StartSubscribeThread();
      this->SleepMainThread();
      return core::ErrorCode::kOk;
}

simba::core::ErrorCode TempApplication::Initialize(
      const std::unordered_map<std::string, std::string>& parms) {

}

void TempApplication::SubCallback(const std::string& ip, const std::uint16_t& port,
      const std::vector<std::uint8_t> data) {
      
      return;
}

simba::core::ErrorCode TempApplication::Subscribe(std::stop_token stoken)
{
      while(true) 
      {
            simba::mw::temp::SubMsgFactory factory;

            SubscribeHeader hdr{5};
            
            std::vector<uint8_t> payload{1,1,1};
            
            std::vector<uint8_t> data = 
                  factory.GetBuffer(std::make_shared<SubscribeHeader>(hdr), payload);
            
            if(auto ret = sock_.Transmit("SIMBA.TEMP.CONTROLLER", 0, data))
            {
                  std::cout << "fail" << std::endl;
            } 
            
            std::this_thread::sleep_for(std::chrono::milliseconds{2000});

            // auto hdr = factory.GetHeader(data);
            // std::vector<uint8_t> payload = factory.GetPayload(data);

            // this->diag_controller->Write(
            //     0x0201, 0x0001, this->conv_.convertUint16ToVector(hdr->GetDtcID()));
      }
}

void TempApplication::StartSubscribeThread() {
    if (sub_thread != nullptr) {
      return;
    }
    this->sub_thread = std::make_unique<std::jthread>(
        [&](std::stop_token stoken) { this->Subscribe(stoken); });
  }

} // namespace temp
} // namespace mw
} // namespace simba