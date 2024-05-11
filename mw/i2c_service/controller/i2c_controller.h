/**
 * @file i2c_controller.h
 * @author Michal Mankowski (m.mankowski2004@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-03-02
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef MW_I2C_SERVICE_CONTROLLER_I2C_CONTROLLER_H_
#define MW_I2C_SERVICE_CONTROLLER_I2C_CONTROLLER_H_
#include <memory>
#include <vector>
#include <string>
#include <utility>
#include <optional>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <condition_variable>  // NOLINT
#include "core/i2c/i2cdriver.hpp"
#include "communication-core/sockets/ipc_socket.h"
#include "mw/i2c_service/data/header.h"
#include "mw/i2c_service/data/i2c_factory.h"


namespace simba {
namespace i2c {
class I2CController{
 private:
  com::soc::IpcSocket sock_;
  std::mutex mtx_;
  std::condition_variable cv_;
  bool response_received_ = false;
  std::vector<uint8_t> response_data_;
  uint16_t service_id;
  void RXCallback(const std::string& ip, const std::uint16_t& port,
                            std::vector<std::uint8_t> data);

 public:
  void Init(uint16_t service_id);
  /**
   * @brief 
   * 
   * @param address (reg,data)
   * @param data 
   * @return core::ErrorCode 
   */
  core::ErrorCode Write(const uint8_t address, const std::vector<uint8_t> data);
  /**
   * @brief 
   * 
   * @param address 
   * @param data 
   * @return core::ErrorCode 
   */
  core::ErrorCode PageWrite(const uint8_t address, const std::vector<uint8_t> data);

  std::optional<std::vector<uint8_t>> Read(const uint8_t address, const uint8_t reg, const uint8_t size = 1);
};
}  // namespace i2c
}  // namespace simba

#endif  // MW_I2C_SERVICE_CONTROLLER_I2C_CONTROLLER_H_
