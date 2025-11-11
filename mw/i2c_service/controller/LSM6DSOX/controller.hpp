/**
 * @file controller.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-04-07
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef MW_I2C_SERVICE_CONTROLLER_LSM6DSOX_CONTROLLER_HPP_
#define MW_I2C_SERVICE_CONTROLLER_LSM6DSOX_CONTROLLER_HPP_
#include <memory>
#include "core/json/json_parser.h"
#include "ara/log/logging_menager.h"
#include "mw/i2c_service/controller/i2c_controller.h"
#include "mw/i2c_service/controller/LSM6DSOX/consts.hpp"
namespace srp {
namespace i2c {

class LSM6DSOX {
 private:
    std::unique_ptr<II2CController> i2c_;
    float accelSensitivity;
    float gyroSensitivity;

    int32_t GetAccelSensitivity(const ACCEL_FULL_SCALE scale);
    int32_t GetGyroSensitivity(const GYRO_FULL_SCALE scale);
    core::ErrorCode InitializeGyro(const config_t& config);
    core::ErrorCode InitializeAccel(const config_t& config);
 public:
    LSM6DSOX() = default;
    ~LSM6DSOX() = default;
    core::ErrorCode Initialize(std::unique_ptr<II2CController> i2c, const config_t& config);
    std::optional<SensorData> ReadGyroData();
    std::optional<SensorData> ReadAccelData();
};
}  // namespace i2c
}  // namespace srp

#endif  // MW_I2C_SERVICE_CONTROLLER_LSM6DSOX_CONTROLLER_HPP_
