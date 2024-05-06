/**
 * @file i2c_controller.cpp
 * @author Michal Mankowski (m.mankowski2004@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-03-14
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <sys/stat.h>
#include <unistd.h>
#include "mw/i2c_service/controller/i2c_controller.h"
#include "core/logger/Logger.h"

namespace simba {
namespace i2c {

namespace {
    const constexpr char* I2C_IPC_ADDR = "SIMBA.I2C";
}
static bool SocketExist(const std::string path) {
  struct stat buffer;
  return (stat(path.c_str(), &buffer) == 0);
}


I2CController::I2CController() {
    this->my_addr_ipc = I2C_IPC_ADDR + '.' + std::to_string(this->service_id);
    AppLogger::Warning(my_addr_ipc);
}
core::ErrorCode I2CController::Write(const uint8_t address, const std::vector<uint8_t> data) {
    AppLogger::Debug("Sent i2c msg");
    auto hdr = Header(ACTION::Write, address, this->service_id);
    auto buf = I2CFactory::GetBuffer(std::make_shared<Header>(hdr), data);
    return this->sock_.Transmit(I2C_IPC_ADDR, 0, buf);
}
core::ErrorCode I2CController::PageWrite(const uint8_t address, const std::vector<uint8_t> data) {
    AppLogger::Debug("Sent i2c msg");
    auto hdr = Header(ACTION::PageWrite, address, this->service_id);
    auto buf = I2CFactory::GetBuffer(std::make_shared<Header>(hdr), data);
    return this->sock_.Transmit(I2C_IPC_ADDR, 0, buf);
}

std::optional<std::vector<uint8_t>> I2CController::Read(const uint8_t address, const uint8_t reg, const uint8_t size) {
    AppLogger::Warning("Sent i2c msg");
    auto buf = I2CFactory::GetBuffer(
                std::make_shared<Header>(ACTION::ReadWrite, address, this->service_id), {reg, size});
    auto err = this->sock_.Transmit(I2C_IPC_ADDR, 0, buf);
    AppLogger::Warning("Wait for response");
    return ReceiveFunc(this->my_addr_ipc);
}
std::optional<std::vector<uint8_t>> I2CController::ReceiveFunc(std::string socketPath) {
  int server_sock, len, rc;
  int bytes_rec = 0;
  struct sockaddr_un server_sockaddr, peer_sock;
memset(&server_sockaddr, 0, sizeof(struct sockaddr_un));
  server_sock = socket(AF_UNIX, SOCK_DGRAM, 0);
  if (server_sock == -1) {
    return {};
  }
  umask(0);
  server_sockaddr.sun_family = AF_UNIX;
  if (SocketExist("/run/" + socketPath)) {
    remove(("/run/" + socketPath).c_str());
  }
  strcpy(server_sockaddr.sun_path,             // NOLINT
         ("/run/" + socketPath).c_str());  // NOLINT
  len = sizeof(server_sockaddr);
  unlink(("/run/" + socketPath).c_str());
  struct timeval tv;
  tv.tv_sec = 1;
  tv.tv_usec = 0;
  setsockopt(server_sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
  rc = bind(server_sock, (struct sockaddr*)&server_sockaddr, len);
  if (rc == -1) {
    return {};
  }
  std::array<char, 256 * 2> buffor;
  uint8_t t = 0;
  do {
    bytes_rec =
        recvfrom(server_sock, reinterpret_cast<char*>(&buffor), 256 * 2, 0,
                 (struct sockaddr*)&peer_sock, (socklen_t*)&len);  // NOLINT
  } while (bytes_rec >= 0 || t > 5);
    return std::vector<uint8_t>{std::begin(buffor), std::end(buffor)};
}

}  // namespace i2c
}  // namespace simba
