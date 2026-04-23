/**
 * @file common.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2026-04-20
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#ifndef CORE_APP_HEARTBEAT_COMMON_HPP_
#define CORE_APP_HEARTBEAT_COMMON_HPP_
#include <cstdint>

namespace srp {
namespace core {
namespace hb {

enum SERVICES_e: uint32_t {
// EC
    SERVICE_EC_TIMESTAMP_MW = 1,
    SERVICE_EC_I2C_MW       = 2,
    SERVICE_EC_GPIO_MW      = 4,
    SERVICE_EC_TEMP_MW      = 8,
    SERVICE_EC_SERVO        = 16,
    SERVICE_EC_ENG          = 32,
    SERVICE_EC_ENV          = 64,
    SERVICE_EC_LOGGER       = 128,
    SERVICE_EC_STAT         = 256,
    SERVICE_EC_IGNITER      = 512,

    // FC
    SERVICE_FC_TIMESTAMP_MW = 1024,
    SERVICE_FC_I2C_MW       = 2048,
    SERVICE_FC_GPIO_MW      = 4096,
    SERVICE_FC_TEMP_MW      = 8192,
    SERVICE_FC_ENV          = 16384,
    SERVICE_FC_GPS          = 32768,
    SERVICE_FC_REC          = 65536,
    SERVICE_FC_RADIO        = 131072,
    SERVICE_FC_MAIN         = 262144,
    SERVICE_FC_STAT         = 524288
};

}  // namespace hb
}  // namespace core
}  // namespace srp

#endif  // CORE_APP_HEARTBEAT_COMMON_HPP_