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
  std::mutex mtx;
  std::vector<uint8_t> received_data_;
  I2CFactory factory_;
  uint16_t service_id;
  void ReceiveCallback(const std::string& ip,
  const std::uint16_t& port, const std::vector<std::uint8_t> data);
  std::condition_variable data_condition;

 public:
  I2CController();
  void Init(uint16_t service_id) {
    this->service_id = service_id;
  }
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
  std::vector<uint8_t> Read(const uint8_t address, const std::vector<uint8_t> reg);
  std::optional<uint8_t> Read(const uint8_t address, const uint8_t reg);
};
}  // namespace i2c
}  // namespace simba

#endif  // MW_I2C_SERVICE_CONTROLLER_I2C_CONTROLLER_H_
