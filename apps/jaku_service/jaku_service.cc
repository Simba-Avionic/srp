
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
        std::make_shared<com::someip::MethodProxyBase>("JakuApp/diodeMethod");
    AppLogger::Info("diodeMethod done");
    com->Add(diode_method);
    AppLogger::Info("diodeMethod com added");
    diode_method->StartFindService();
    AppLogger::Info("diode method StartFindService");
    while(true){
        this->temp_event_1->SetValue(temp_vector_1);
        AppLogger::Info("sent temp1");
        this->temp_event_2->SetValue(temp_vector_2);
        AppLogger::Info("sent temp 2");
        this->temp_event_3->SetValue(temp_vector_3);
        AppLogger::Info("sent temp 3");
        if (diodeCounter == 3){
            AppLogger::Info("Checked diode counter");
            if(sendDiode){
                AppLogger::Info("Sent diode");
                auto result = diode_method->Get({2,0});
                AppLogger::Info("got result from diodeMethod");
                if(result.has_value()){
                    AppLogger::Info("succes has value");
                    success = result.value();
                    AppLogger::Info("success");
                }
                else{
                    AppLogger::Info("Response not received");
                }
                sendDiode = false;
            }
            else{
                AppLogger::Info("dtc controller");
                auto result = diode_method->Get({2,1});
                AppLogger::Info("result2");
                if(result.has_value()){
                    success = result.value();
                    AppLogger::Info("success2");
                }
                else{
                    AppLogger::Info("Response not received");
                }
                sendDiode = true;
            }
            diodeCounter = 0;
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
        diodeCounter++;
    }
    AppLogger::Info("Jaku end");
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
    return core::ErrorCode::kOk;
}

void JakuService::TempRxCallback(const std::string& ip, const std::uint16_t& port,
                                const std::vector<std::uint8_t> data) {

mw::temp::TempReadingMsgFactory factory_;
auto hdrs = factory_.GetPayload(data);
for (auto &hdr : hdrs) {
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
