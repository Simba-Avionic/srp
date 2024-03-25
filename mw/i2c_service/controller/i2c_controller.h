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
#include <condition_variable>  // NOLINT
#include "core/i2c/i2cdriver.h"
#include "communication-core/sockets/ipc_socket.h"
#include "mw/i2c_service/data/header.h"
#include "mw/i2c_service/data/i2c_factory.h"


namespace simba {
class I2CController{
 private:
  com::soc::IpcSocket sock_;
  std::mutex data_mutex;
  std::mutex mtx;
  std::vector<uint8_t> received_data_;
  uint8_t service_id;
  void ReceiveCallback(const std::string& ip,
  const std::uint16_t& port, const std::vector<std::uint8_t> data);
  bool data_ready;
  std::condition_variable data_condition;
 public:
  explicit I2CController(uint8_t service_id);
  core::ErrorCode Write(const uint8_t address, const uint8_t reg, std::vector<uint8_t> data);
  std::vector<uint8_t> Read(const uint8_t address, const uint8_t reg);
};
}  // namespace simba

#endif  // MW_I2C_SERVICE_CONTROLLER_I2C_CONTROLLER_H_
