/**
 * @file header.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-03-08
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef MW_GPIO_SERVER_DATA_HEADER_HPP_
#define MW_GPIO_SERVER_DATA_HEADER_HPP_

#include <cstdint>

#include "communication-core/network-data/network_data_structure.h"
#include "communication-core/network-data/network_data_type.h"


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

class Header : public com::core::network::NetworkDataStructure {
 private:
    com::core::network::uint8_t action;
    com::core::network::uint8_t pin_id;
    com::core::network::uint8_t value;
    void SetData();
 public:
    uint16_t GetActuatorID() { return this->pin_id.Get(); }
    uint8_t GetValue() { return this->value.Get(); }
    ACTION GetAction() { return static_cast<ACTION>(this->action.Get());}
    void SetValue(uint8_t value);
    void SetAction(ACTION action);
    void SetPinID(uint8_t pin_id);
    Header(uint8_t actuator_id, uint8_t value, ACTION action);
};

}  // namespace gpio
}  // namespace srp




#endif  // MW_GPIO_SERVER_DATA_HEADER_HPP_
