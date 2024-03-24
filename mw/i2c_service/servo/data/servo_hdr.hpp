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

class ServoHdr : public com::core::network::NetworkDataStructure {
 private:
    com::core::network::uint16_t service_id;
    com::core::network::uint8_t actuator_id;
    com::core::network::uint8_t position;
    void SetData();
 public:
    uint16_t GetServiceID() { return service_id.Get(); }
    uint8_t GetActuatorID() { return actuator_id.Get(); }
    uint8_t GetPosition() { return position.Get(); }
    ServoHdr(uint16_t service_id, uint8_t actuator_id, uint8_t position);
};

}  // namespace i2c
}  // namespace simba




#endif  // MW_I2C_SERVICE_DATA_SERVO_HDR_HPP_
