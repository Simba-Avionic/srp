/**
 * @file gpio_controller.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-03-08
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "gpio_controller.hpp"
#include "mw/gpio_server/data/resHeader.hpp"

namespace simba {
namespace gpio {

void GPIOController::Init(uint16_t service_id) {
    this->service_id = service_id;
}

std::optional<std::vector<uint8_t>> GPIOController::ReceiveData(std::string path, uint8_t buffer_size) {
    int sockrc;
    struct sockaddr_un servaddr;
    sockrc = socket(AF_UNIX, SOCK_DGRAM, 0);
    servaddr.sun_family = AF_UNIX;
    strncpy(servaddr.sun_path, ("/run/"+path).c_str(), sizeof(servaddr.sun_path)-1);
    bind(sockrc, (const struct sockaddr *)&servaddr, sizeof(servaddr));
    std::vector<uint8_t> buffer(buffer_size);
    recvfrom(sockrc, buffer.data(), buffer.size(), MSG_WAITALL, NULL, NULL);
    close(sockrc);
    return std::optional<std::vector<uint8_t>>(buffer);
}

core::ErrorCode GPIOController::SetPinValue(uint16_t pinID, uint8_t value) {
    static gpio::Header hdr(this->service_id, pinID, value);
    gpio::ResHeader resHdr(0, 0);
    this->sock_.Transmit("SIMBA.GPIO.SET", 0, hdr.GetBuffor());
    auto data = this->ReceiveData("SIMBA.GPIO.SET."+std::to_string(this->service_id), 3);
    if (data.has_value()) {
        resHdr.SetBuffor(data.value());
    }
    if (resHdr.GetValue() == value) {
        return core::ErrorCode::kOk;
    }
    return core::ErrorCode::kError;
}

uint8_t GPIOController::GetPinValue(uint16_t pinID) {
}


}  // namespace gpio
}  // namespace simba
