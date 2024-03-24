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

#ifndef CORE_SERVOCONTROLLER_SERVOCONTROLLER_HPP_
#define CORE_SERVOCONTROLLER_SERVOCONTROLLER_HPP_

#include "IservoController.hpp"
#include "communication-core/sockets/ipc_socket.h"

namespace simba {
namespace core {

class ServoController: public IServoController{
 public:
 /**
  * @brief Set the Servo Pos object
  * 
  * @param actuatorID 
  * @param position 0-4096
  * @return ErrorCode 
  */
    ErrorCode SetServoPos(uint8_t actuatorID, uint8_t position) override;
 private:
    com::soc::IpcSocket sock_;
    
};
}  // namespace core
}  // namespace simba




#endif // CORE_SERVOCONTROLLER_SERVOCONTROLLER_HPP_