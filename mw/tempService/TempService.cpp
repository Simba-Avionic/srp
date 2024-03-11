/**
 * @file TempSerivce.cpp
 * @author Maciek Matuszewski (maciej.matuszewsky@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-03-02
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "TempService.h"
#include <iostream>

namespace simba {
namespace mw {
namespace temp {

simba::core::ErrorCode TempApplication::Run(std::stop_token token) {
      this->StartSubscribeThread();
      this->sock_.StartRXThread();
      this->SleepMainThread();
      return core::ErrorCode::kOk;
}

simba::core::ErrorCode TempApplication::Initialize(
      const std::unordered_map<std::string, std::string>& parms) {
      if (auto ret = this->sock_.Init(
        com::soc::SocketConfig("Engine.16", 0, 0))) {
            AppLogger::Error("Couldn't initialize socket!");
            return ret;
      }
      this->sock_.SetRXCallback(
        std::bind(&simba::mw::temp::TempApplication::SubCallback, this,
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

void TempApplication::SubCallback(const std::string& ip, const std::uint16_t& port,
      const std::vector<std::uint8_t> data) {
      simba::mw::temp::TempReadingMsgFactory factory;
      std::vector<TempReading> readings = factory.GetPayload(data);

      for (auto reading : readings) {
            AppLogger::Info(std::to_string(reading.first) +
                  " " + std::to_string(reading.second));
      }
      return;
}

simba::core::ErrorCode TempApplication::Subscribe(std::stop_token stoken) {
      while (true) {
            simba::mw::temp::SubMsgFactory factory;
            SubscribeHeader hdr{0x0010};

            std::vector<uint8_t> data =
                  factory.GetBuffer(std::make_shared<SubscribeHeader>(hdr), {});

            if (auto ret = sock_.Transmit("SIMBA.TEMP.CONTROLLER", 0, data)) {
                  std::cout << "fail" << std::endl;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds{1000});
      }
}

void TempApplication::StartSubscribeThread() {
    if (sub_thread != nullptr) {
      return;
    }
    this->sub_thread = std::make_unique<std::jthread>(
        [&](std::stop_token stoken) { this->Subscribe(stoken); });
}

}  // namespace temp
}  // namespace mw
}  // namespace simba
