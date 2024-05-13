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
    uint8_t success;
    auto diode_method = 
        std::make_shared<com::someip::MethodProxyBase>("HomeworkApp/diodeMethod");
    com->Add(diode_method);
    diode_method->StartFindService();
    while(true){
        this->temp_event_1->SetValue(temp_vector_1);
        this->temp_event_2->SetValue(temp_vector_2);
        this->temp_event_3->SetValue(temp_vector_3);
        if (diodeCounter == 3){
            if(sendDiode){
                success = diode_method->Get({0});
                sendDiode = false;
            }
            else{
                success = diode_method->Get({1});
                sendDiode = true;
            }
            diodeCounter = 0;
            if success[0] == 1{
                AppLogger::Info("Diode change successfull");
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
    this->temp_event_1 = std::make_shared<com::someip::EventSkeleton>("JakuApp/newTempEvent_1");
    this->temp_event_2 = std::make_shared<com::someip::EventSkeleton>("JakuApp/newTempEvent_2");
    this->temp_event_3 = std::make_shared<com::someip::EventSkeleton>("JakuApp/newTempEvent_3");
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