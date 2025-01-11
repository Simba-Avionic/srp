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
#include <optional>
#include "mw/i2c_service/data/header.h"
#include "mw/i2c_service/data/i2c_factory.h"
#include "communication-core/sockets/stream_ipc_socket.h"

#include "mw/i2c_service/controller/Ii2c_controller.h"


namespace srp {
namespace i2c {
class I2CController: public II2CController{
 private:
  std::unique_ptr<com::soc::ISocketStream> sock_;

 protected:
  std::optional<std::vector<uint8_t>> SendData(ACTION action,
                        uint8_t address, const std::vector<uint8_t>& payload) override;

 public:
  core::ErrorCode Init(std::unique_ptr<com::soc::ISocketStream> socket
                            = std::make_unique<com::soc::StreamIpcSocket>()) override;
  /**
   * @brief 
   * 
   * @param address
   * @param data (reg,data)
   * @return core::ErrorCode 
   */
  core::ErrorCode Write(const uint8_t address, const std::vector<uint8_t> data) override;
  /**
   * @brief 
   * 
   * @param address 
   * @param data 
   * @return core::ErrorCode 
   */
  core::ErrorCode PageWrite(const uint8_t address, const std::vector<uint8_t> data) override;

  std::optional<std::vector<uint8_t>> Read(const uint8_t address, const uint8_t reg, const uint8_t size = 1) override;
  std::optional<std::vector<uint8_t>> WriteRead(const uint8_t address,
                                                        const uint8_t WriteData, const uint8_t ReadSize = 1) override;
};
}  // namespace i2c
}  // namespace srp

#endif  // MW_I2C_SERVICE_CONTROLLER_I2C_CONTROLLER_H_
