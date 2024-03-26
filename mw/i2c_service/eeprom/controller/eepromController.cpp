/**
 * @file eepromController.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-03-25
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <memory>
#include <string>
#include <utility>

#include "eepromController.hpp"

namespace simba {
namespace core {

EepromController::EepromController(std::unique_ptr<com::soc::IpcSocket> sock):sock_(std::move(sock)) {}

std::vector<uint8_t> SmallIpcReceiver(std::string sock_path) {
  int server_sock, len, rc;
  int bytes_rec = 0;
  struct sockaddr_un server_sockaddr, peer_sock;
  memset(&server_sockaddr, 0, sizeof(struct sockaddr_un));
  server_sock = socket(AF_UNIX, SOCK_DGRAM, 0);
  if (server_sock == -1) {
    return {};
  }
  server_sockaddr.sun_family = AF_UNIX;
  strcpy(server_sockaddr.sun_path,             // NOLINT
         ("/run/SIMBA.EEPROM." + sock_path).c_str());  // NOLINT
  len = sizeof(server_sockaddr);
  unlink(("/run/SIMBA.EEPROM." + sock_path).c_str());
  struct timeval tv;
  tv.tv_sec = 2;
  tv.tv_usec = 0;
  setsockopt(server_sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
  rc = bind(server_sock, (struct sockaddr*)&server_sockaddr, len);
  if (rc == -1) {
    return;
  }
    std::array<char, 256 * 2> buffor;
    do {
    bytes_rec =
            recvfrom(server_sock, reinterpret_cast<char*>(&buffor), 256 * 2, 0,
                 (struct sockaddr*)&peer_sock, (socklen_t*)&len);  // NOLINT
    } while (bytes_rec >= 0);
    return std::vector<uint8_t>{buffor.begin(), buffor.begin() + bytes_rec};
}

std::optional<uint8_t> EepromController::ReadData(uint8_t address, uint8_t reg) {
    auto hdr = i2c::EepromHdr {this->appID, this->transferID, i2c::operation_t::READ, address};
    auto data = this->factory_.GetBuffer(std::make_shared<i2c::EepromHdr>(hdr), {reg});
    this->sock_->Transmit("SIMBA.EEPROM", 0, data);
    auto data = SmallIpcReceiver(std::to_string(this->appID)+"."+std::to_string(this->transferID));
    this->transferID++;
    if (data.size() == 0) {
        return {};
    }
    auto hdr = this->factory_.GetHeader(data);
    auto payload = this->factory_.GetPayload(data);
    if (hdr.GetOperationType() != i2c::operation_t::RES) {
        return {};
    }
    return payload[0];
}
std::vector<uint8_t> EepromController::ReadData(uint8_t address) {
    auto hdr = i2c::EepromHdr {this->appID, this->transferID, i2c::operation_t::READ, address};
    auto data = this->factory_.GetBuffer(std::make_shared<i2c::EepromHdr>(hdr), {});
    this->sock_->Transmit("SIMBA.EEPROM", 0, data);
    auto data = SmallIpcReceiver(std::to_string(this->appID)+"."+std::to_string(this->transferID));
    this->transferID++;
    if (data.size() == 0) {
        return {};
    }
    auto hdr = this->factory_.GetHeader(data);
    auto payload = this->factory_.GetPayload(data);
    if (hdr.GetOperationType() != i2c::operation_t::RES) {
        return {};
    }
    return payload;
}
void EepromController::Init(uint16_t app_id) {
    this->appID = app_id;
}
ErrorCode EepromController::WriteData(uint8_t address, uint8_t reg, uint8_t data) {

}
ErrorCode EepromController::WriteData(uint8_t address, std::vector<uint8_t> data) {

}

}  // namespace core
}  // namespace simba
