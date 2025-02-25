/**
 * @file i2c_factory.h
 * @author Michal Mankowski (m.mankowski2004@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-03-14
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef MW_I2C_SERVICE_DATA_I2C_FACTORY_H_
#define MW_I2C_SERVICE_DATA_I2C_FACTORY_H_

#include <cstdint>
#include <vector>
#include <memory>
#include <optional>
#include "mw/i2c_service/data/header.h"
namespace srp {
namespace i2c {
class I2CFactory {
 public:
    static std::vector<uint8_t> GetBuffer(std::shared_ptr<Header> header, const std::vector<uint8_t>& payload);
    static std::shared_ptr<Header> GetHeader(const std::vector<uint8_t>& raw_data);
    static std::optional<std::vector<uint8_t>> GetPayload(const std::vector<uint8_t>& raw_data);
};
}  // namespace i2c
}  // namespace srp
#endif  // MW_I2C_SERVICE_DATA_I2C_FACTORY_H_
