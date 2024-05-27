/**
 * @file jaku_service.cc
 * @author Jacek Kukawski (jacekka6@gmail.com)
 * @brief class describing jaku service
 * @version 1.0
 * @date 2024-05-11
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "apps/jaku_service/jaku_service.h"
#include "mw/temp/temp_reading_msg/temp_reading_msg_factory.h"
#include "communication-core/someip-controller/method_proxy.h"
#include <chrono>
#include <thread>

namespace simba {
namespace jakuService {

std::vector<uint8_t> floatToBytes(float value) {
    std::vector<uint8_t> bytes(sizeof(float));
    std::memcpy(bytes.data(), &value, sizeof(float));
    return bytes;
}

core::ErrorCode JakuService::Run(std::stop_token token) {
    bool sendDiode = true;
    uint8_t diodeCounter = 0;
    std::vector<uint8_t> success;
    auto diode_method = 
        std::make_shared<com::someip::MethodProxyBase>("diodeMethod");
    com->Add(diode_method);
    diode_method->StartFindService();
    while(true){
        this->temp_event_1->SetValue(temp_vector_1);
        this->temp_event_2->SetValue(temp_vector_2);
        this->temp_event_3->SetValue(temp_vector_3);
        if (diodeCounter == 3){
            if(sendDiode){
                
                auto result = diode_method->Get({0});
                if(result.has_value()){
                    success = result.value();
                }
                else{
                    AppLogger::Info("Response not received");
                }
                sendDiode = false;
            }
            else{
                auto result = diode_method->Get({1});
                if(result.has_value()){
                    success = result.value();
                }
                else{
                    AppLogger::Info("Response not received");
                    success = new std::vector<uint8_t>{0};
                }
                sendDiode = true;
            }
            diodeCounter = 0;
            if (success[0] == 1){
                AppLogger::Info("Diode change successful");
            }
            else{
                AppLogger::Info("Diode error");
            }
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
        diodeCounter++;
    }
    return core::ErrorCode::kOk;
}

core::ErrorCode JakuService::Initialize(
    const std::unordered_map<std::string, std::string>& params){
    this->temp_event_1 = std::make_shared<com::someip::EventSkeleton>("JakuApp/sentTempEvent_1");
    this->temp_event_2 = std::make_shared<com::someip::EventSkeleton>("JakuApp/sentTempEvent_2");
    this->temp_event_3 = std::make_shared<com::someip::EventSkeleton>("JakuApp/sentTempEvent_3");
    com->Add(temp_event_1);
    com->Add(temp_event_2);
    com->Add(temp_event_3);
    core::ErrorCode res;
    do {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        res = this->temp_.Init(514, std::bind(&JakuService::TempRxCallback,
            this, std::placeholders::_1, std::placeholders::_2,
                                        std::placeholders::_3));
    } while (res != core::ErrorCode::kOk);
    }
    return core::ErrorCode::kOk;
}

bool EnvService::CheckTempError(const double &value) {
    if (value > 30) {
        dtc_temp_error->Failed();
        return false;
    } else {
        dtc_temp_error->Pass();
        return true;
    }
}

void JakuService::TempRxCallback(const std::string& ip, const std::uint16_t& port,
                                const std::vector<std::uint8_t> data) {

mw::temp::TempReadingMsgFactory factory_;
auto hdrs = factory_.GetPayload(data);
for (auto &hdr : hdrs) {
    this->CheckTempError(hdr.second);
    AppLogger::Info("Received temp data: " + std::to_string(hdr.first) + " " + std::to_string(hdr.second));
    switch (hdr.first)
    {
    case 0:
        this->temp_vector_1 = floatToBytes(hdr.second);
        break;
    case 1:
        this->temp_vector_2 = floatToBytes(hdr.second);
        break;
    case 2:
        this->temp_vector_3 = floatToBytes(hdr.second);
        break;
    default:
        AppLogger::Error("Wrong id" + std::to_string(hdr.first));
        break;
    };
}
                                }
} // namespace jakuService
} // namespace simba