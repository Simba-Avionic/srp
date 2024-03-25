/**
 * @file eeprom_hdr.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-03-25
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef MW_I2C_SERVICE_EEPROM_DATA_EEPROM_HDR_HPP_
#define MW_I2C_SERVICE_EEPROM_DATA_EEPROM_HDR_HPP_

#include <cstdint>

#include "communication-core/network-data/network_data_structure.h"
#include "communication-core/network-data/network_data_type.h"


namespace simba {
namespace i2c {

enum operation_t {
  READ = 0x00,
  WRITE = 0x01,
  RES = 0x02
};

class EepromHdr : public com::core::network::NetworkDataStructure {
 private:
    com::core::network::uint16_t client_id{0};
    com::core::network::uint8_t transmition_id{0};
    com::core::network::uint8_t operation{0};
    com::core::network::uint8_t address{0};

    void SetData();
 public:
    uint16_t GetClientID() { return client_id.Get(); }
    uint8_t GetTransmitionID() { return transmition_id.Get(); }
    operation_t GetOperationType() { return static_cast<operation_t>(operation.Get()); }
    uint8_t GetAddress() { return address.Get(); }
    EepromHdr(uint16_t client_id, uint8_t transmition_id, operation_t operation, uint8_t address = 0x00);
    EepromHdr();
};

}  // namespace i2c
}  // namespace simba




#endif  // MW_I2C_SERVICE_EEPROM_DATA_EEPROM_HDR_HPP_
