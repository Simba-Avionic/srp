/**
 * @file dtc_header.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-12-23
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef DIAG_DTC_DATA_DTC_HEADER_HPP_
#define DIAG_DTC_DATA_DTC_HEADER_HPP_

#include <chrono>  // NOLINT
#include <vector>

#include "communication-core/network-data/network_data_structure.h"
#include "communication-core/network-data/network_data_type.h"

namespace simba {
namespace diag {
namespace dtc {

class DtcHeader : public com::core::network::NetworkDataStructure {
 private:
  // numer błędu
    com::core::network::uint16_t dtc_id_;
    // flagi błędu
    com::core::network::uint8_t dtc_status_;
    com::core::network::uint8_t lenght_;

 public:
  DtcHeader(const uint16_t &dtc_id, const uint8_t &dtc_status);
  DtcHeader();

  uint16_t GetDtcID();
  uint8_t GetDtcStatus();
  uint8_t GetLength();

  void SetData() {
    this->AddData(&dtc_id_);
    this->AddData(&dtc_status_);
    this->AddData(&lenght_);
  }

   /**
    * @brief Set the Length object
    * 
    * @param value  (set only lenght of payload )
    */
  void SetLength(const uint8_t& value);

  void SetData() {
    this->AddData(&dtc_id_);
    this->AddData(&dtc_status_);
    this->AddData(&lenght_);
  }
};

}  // namespace dtc
}  // namespace diag
}  // namespace simba

#endif  // DIAG_DTC_DATA_DTC_HEADER_HPP_
