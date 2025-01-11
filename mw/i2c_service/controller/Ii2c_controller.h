/**
 * @file Ii2c_controller.h
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-06-13
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef MW_I2C_SERVICE_CONTROLLER_II2C_CONTROLLER_H_
#define MW_I2C_SERVICE_CONTROLLER_II2C_CONTROLLER_H_
#include <vector>
#include <optional>
#include <memory>
#include "mw/i2c_service/data/header.h"
#include "communication-core/sockets/stream_ipc_socket.h"

namespace srp {
namespace i2c {
class II2CController{
 protected:
  virtual std::optional<std::vector<uint8_t>> SendData(ACTION action,
                        uint8_t address, const std::vector<uint8_t>& payload) = 0;

 public:
  virtual core::ErrorCode Init(std::unique_ptr<com::soc::ISocketStream> socket) = 0;
  /**
   * @brief  write data to adress, data format is {reg1, data1, reg2, data2 , ...}
   * 
   * @param address
   * @param data (reg,data)
   * @return core::ErrorCode 
   */
  virtual core::ErrorCode Write(const uint8_t address, const std::vector<uint8_t> data) = 0;
/**
 * @brief Write sizeof(vector) data to registers start from reg=0
 * 
 * @param address 
 * @param data 
 * @return core::ErrorCode 
 */
  virtual core::ErrorCode PageWrite(const uint8_t address, const std::vector<uint8_t> data) = 0;

  /**
   * @brief Read data from reg start from reg, read n=size registers
   * 
   * @param address 
   * @param reg 
   * @param size 
   * @return std::optional<std::vector<uint8_t>> 
   */
  virtual std::optional<std::vector<uint8_t>> Read(const uint8_t address,
                                                                const uint8_t reg, const uint8_t size = 1) = 0;
  /**
   * @brief 
   * 
   * @param address 
   * @param WriteData 
   * @param ReadSize 
   * @return std::optional<std::vector<uint8_t>> 
   */
  virtual std::optional<std::vector<uint8_t>> WriteRead(const uint8_t address,
                                                        const uint8_t WriteData, const uint8_t ReadSize = 1) = 0;
  virtual ~II2CController() = default;
};
}  // namespace i2c
}  // namespace srp

#endif  // MW_I2C_SERVICE_CONTROLLER_II2C_CONTROLLER_H_
