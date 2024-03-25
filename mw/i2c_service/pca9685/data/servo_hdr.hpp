/**
 * @file servo_hdr.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-03-24
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef MW_I2C_SERVICE_DATA_SERVO_HDR_HPP_
#define MW_I2C_SERVICE_DATA_SERVO_HDR_HPP_

#include <cstdint>

#include "communication-core/network-data/network_data_structure.h"
#include "communication-core/network-data/network_data_type.h"


namespace simba {
namespace i2c {

enum smode_t {
  AUTO = 0x00,
  MAN = 0x01
};

class ServoHdr : public com::core::network::NetworkDataStructure {
 private:
    com::core::network::uint8_t actuator_id;
    com::core::network::uint16_t position;
    com::core::network::uint8_t mode;

    void SetData();
 public:
    uint8_t GetActuatorID() { return actuator_id.Get(); }
    uint16_t GetPosition() { return position.Get(); }
    smode_t GetMode() { return static_cast<smode_t>(mode.Get()); }
    ServoHdr(uint8_t actuator_id, uint16_t position, smode_t mode = smode_t::AUTO);
};

}  // namespace i2c
}  // namespace simba




#endif  // MW_I2C_SERVICE_DATA_SERVO_HDR_HPP_
