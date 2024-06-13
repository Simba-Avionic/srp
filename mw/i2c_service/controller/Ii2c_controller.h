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
#include "mw/i2c_service/data/header.h"


namespace simba {
namespace i2c {
class II2CController{
 protected:
  virtual std::optional<std::vector<uint8_t>> SendData(ACTION action,
                        uint8_t address, const std::vector<uint8_t>& payload) = 0;

 public:
  /**
   * @brief 
   * 
   * @param address
   * @param data (reg,data)
   * @return core::ErrorCode 
   */
  virtual core::ErrorCode Write(const uint8_t address, const std::vector<uint8_t> data) = 0;
  /**
   * @brief 
   * 
   * @param address 
   * @param data 
   * @return core::ErrorCode 
   */
  virtual core::ErrorCode PageWrite(const uint8_t address, const std::vector<uint8_t> data) = 0;

  virtual std::optional<std::vector<uint8_t>> Read(const uint8_t address,
                                                                const uint8_t reg, const uint8_t size = 1) = 0;
  virtual std::optional<std::vector<uint8_t>> WriteRead(const uint8_t address,
                                                        const uint8_t WriteData, const uint8_t ReadSize = 1) = 0;
  virtual ~II2CController() = default;
};
}  // namespace i2c
}  // namespace simba

#endif  // MW_I2C_SERVICE_CONTROLLER_II2C_CONTROLLER_H_
