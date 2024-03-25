/**
 * @file servoController.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-03-24
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef MW_I2C_SERVICE_PCA9685_CONTROLLER_SERVOCONTROLLER_HPP_
#define MW_I2C_SERVICE_PCA9685_CONTROLLER_SERVOCONTROLLER_HPP_

#include "communication-core/sockets/ipc_socket.h"

namespace simba {
namespace core {

class ServoController{
 public:
 /**
  * @brief Set the Servo Pos object
  * 
  * @param actuatorID 
  * @param position 0-4096
  * @return ErrorCode 
  */
    ErrorCode SetServoPos(uint8_t actuatorID, uint8_t position);
    ErrorCode MANSetServoPos(uint8_t actuatorID, uint16_t position);
    void Init(uint16_t app_id);
 private:
    com::soc::IpcSocket sock_;
    uint16_t app_id;
};
}  // namespace core
}  // namespace simba




#endif  // MW_I2C_SERVICE_PCA9685_CONTROLLER_SERVOCONTROLLER_HPP_
