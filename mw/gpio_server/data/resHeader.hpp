/**
 * @file resHeader.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-03-08
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef MW_GPIO_SERVER_DATA_RESHEADER_HPP_
#define MW_GPIO_SERVER_DATA_RESHEADER_HPP_

#include <cstdint>

#include "communication-core/network-data/network_data_structure.h"
#include "communication-core/network-data/network_data_type.h"


namespace simba {
namespace gpio {

class ResHeader : public com::core::network::NetworkDataStructure {
 private:
    com::core::network::uint16_t pin_id;
    com::core::network::uint8_t value;
    void SetData();
 public:
    uint16_t GetPinID() { return this->pin_id.Get(); }
    uint8_t GetValue() { return this->value.Get(); }
    ResHeader(uint16_t pin_id, uint8_t value);
};

}  // namespace gpio
}  // namespace simba




#endif  // MW_GPIO_SERVER_DATA_RESHEADER_HPP_
