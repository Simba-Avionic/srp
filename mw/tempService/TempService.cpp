#include "TempService.h"
#include <iostream>

namespace simba {
namespace mw {
namespace temp {


simba::core::ErrorCode TempApplication::Run(std::stop_token token) {

      this->temp_sock_.StartRXThread();
      this->sub_sock_.StartRXThread();
      std::cout << "Started TempService!" << std::endl;

      this->SleepMainThread();
      return core::ErrorCode::kOk;
}

simba::core::ErrorCode TempApplication::Initialize(
      const std::unordered_map<std::string, std::string>& parms) {
      
      if (auto ret = this->temp_sock_.Init(com::soc::SocketConfig(
      kTempServiceName, 0, 0))) 
      {
            AppLogger::Error("Couldn't initialize " + 
                  std::string(kTempServiceName) + "socket!");
            return ret;
      }

      if (auto ret = this->temp_sock_.Init(com::soc::SocketConfig(
      kTempServiceSubName, 0, 0)))
      {
            AppLogger::Error("Couldn't initialize " + 
                  std::string(kTempServiceSubName) + "socket!");
            return ret; 
      }
      
      // receiving temperature readings
      this->temp_sock_.SetRXCallback(
            std::bind(&simba::mw::temp::TempApplication::TempCallback, this, 
                  std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

      // receiving subscribtion calls
      this->sub_sock_.SetRXCallback(
            std::bind(&simba::mw::temp::TempApplication::SubCallback, this, 
                  std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

      return simba::core::ErrorCode::kOk;
}

void TempApplication::TempCallback(const std::string& ip, const std::uint16_t& port,
      const std::vector<std::uint8_t> data) {
      
      std::cout << "Message received! Content: " << std::endl;

      // diag::dtc::DtcMsgFactory factory;
      // auto hdr = factory.GetHeader(data);
      // std::vector<uint8_t> payload = factory.GetPayload(data);

      // this->diag_controller->Write(
      //       0x0201, 0x0001, this->conv_.convertUint16ToVector(hdr->GetDtcID()));

      return;
}


void TempApplication::SubCallback(const std::string& ip, const std::uint16_t& port,
      const std::vector<std::uint8_t> data) {
      
      return;
}

// TODO: REGISTER CLIENTS (SUBSCRIBERS)
// we've got a socket for service and in the callback function
// it should create user and send data to it

// TODO: GET DATA FROM TEMPERATURE SENSORS
// from TempController idk yet how - another socket maybe


} // namespace temp
} // namespace mw
} // namespace simba