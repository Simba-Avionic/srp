/**
 * @file enums.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-03-24
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef MW_GPIO_SERVER_DATA_ENUMS_HPP_
#define MW_GPIO_SERVER_DATA_ENUMS_HPP_
#include <cstdint>
namespace srp {
namespace gpio {
enum ACTION : uint8_t {
    SET = 0,
    GET = 1,
    RES = 2,
    SUBSCRIBE = 3,
    UNSUBSCRIBE = 4,
    CALLBACK = 5,
    };
}  // namespace gpio
}  // namespace srp

#endif  // MW_GPIO_SERVER_DATA_ENUMS_HPP_
