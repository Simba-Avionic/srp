/**
 * @file consts.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-04-26
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef MW_I2C_SERVICE_CONTROLLER_LSM6DSOX_CONSTS_HPP_
#define MW_I2C_SERVICE_CONTROLLER_LSM6DSOX_CONSTS_HPP_
#include <cstdint>
namespace srp {
namespace i2c {
enum class ACCEL_SPEED_t: uint8_t {
    k12_5 = 0b10110000,
    k26 =   0b00100000,
    k52 =   0b00110000,
    k104 =  0b01000000,
    k208 =  0b01010000,
    k416 =  0b01100000,
    k833 =  0b01110000,
    k1k66 = 0b10000000,
    k3k33 = 0b10010000,
    k6k66 = 0b10100000
};
enum class ACCEL_FULL_SCALE: uint8_t {
    k2g = 0b00000000,
    k4g = 0b00001000,
    k8g = 0b00001100,
    k16g = 0b00000100
};
enum class GYRO_SPEED_t: uint8_t {
    k12_5 = 0b00010000,
    k26 =   0b00100000,
    k52 =   0b00110000,
    k104 =  0b01000000,
    k208 =  0b01010000,
    k416 =  0b01100000,
    k833 =  0b01110000,
    k1k66 = 0b10000000,
    k3k33 = 0b10010000,
    k6k66 = 0b10100000
};
enum class GYRO_FULL_SCALE: uint8_t {
    k250 = 0b00000000,
    k500 = 0b00000100,
    k1000 = 0b00001000,
    k2000 = 0b00001100
};
struct config_t {
    ACCEL_SPEED_t accel_speed;
    ACCEL_FULL_SCALE accel_scale;
    GYRO_SPEED_t gyro_speed;
    GYRO_FULL_SCALE gyro_scale;
};
struct SensorData {
    float x, y, z;
};
}  // namespace i2c
}  // namespace srp
#endif  // MW_I2C_SERVICE_CONTROLLER_LSM6DSOX_CONSTS_HPP_
