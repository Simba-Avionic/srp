/**
 * @file IservoController.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-03-24
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef CORE_SERVOCONTROLLER_ISERVOCONTROLLER_HPP_
#define CORE_SERVOCONTROLLER_ISERVOCONTROLLER_HPP_

#include "core/common/error_code.h"

namespace simba {
namespace core {

class IServoController{
 public:
    virtual ErrorCode SetServoPos(uint8_t actuatorID, uint8_t position);
};
}  // namespace core
}  // namespace simba


#endif  // CORE_SERVOCONTROLLER_ISERVOCONTROLLER_HPP_
