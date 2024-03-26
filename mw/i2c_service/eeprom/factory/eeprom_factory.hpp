/**
 * @file eeprom_factory.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-03-25
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef MW_I2C_SERVICE_EEPROM_FACTORY_EEPROM_FACTORY_HPP_
#define MW_I2C_SERVICE_EEPROM_FACTORY_EEPROM_FACTORY_HPP_

#include <memory>
#include <vector>

#include "mw/i2c_service/eeprom/data/eeprom_hdr.hpp"

namespace simba {
namespace i2c {

class EepromMsgFactory {
 public:
  std::vector<uint8_t> GetBuffer(std::shared_ptr<i2c::EepromHdr> hdr,
                                        std::vector<uint8_t> payload);

  std::shared_ptr<i2c::EepromHdr> GetHeader(std::vector<uint8_t> raw_data);

  std::vector<uint8_t> GetPayload(std::vector<uint8_t> raw_data);
};
}  // namespace i2c
}  // namespace simba


#endif  // MW_I2C_SERVICE_EEPROM_FACTORY_EEPROM_FACTORY_HPP_

