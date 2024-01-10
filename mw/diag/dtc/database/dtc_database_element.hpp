/**
 * @file dtc_database_element.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-12-23
 *
 * @copyright Copyright (c) 2023
 *
 **/

#ifndef MW_DIAG_DTC_DATABASE_DTC_DATABASE_ELEMENT_HPP_
#define MW_DIAG_DTC_DATABASE_DTC_DATABASE_ELEMENT_HPP_

#include <chrono>  // NOLINT
#include <string>

#include "communication-core/network-data/network_data_structure.h"

namespace simba {
namespace mw {
namespace dtc {


class DtcDatabaseElement : public com::core::network::NetworkDataStructure {
 private:
    uint16_t dtc_error_code_;
    std::string details_;
    uint8_t status_;
 public:
    DtcDatabaseElement(uint16_t dtc_error_code_,std::string details_,
            uint8_t status_=0x0);

    void SetDtcErrorCode(const uint16_t& dtc_error_code_);
    void SetDetails(const std::string& details);
    void SetStatus(const uint8_t& status);

    uint16_t GetDtcErrorCode();
    std::string GetDetails();
    uint8_t GetStatus();

  uint16_t GetService_id();
  std::string GetDetails();
  DtcErrorStatus_t GetStatus();
};

}  // namespace dtc
}  // namespace mw
}  // namespace simba

#endif  // MW_DIAG_DTC_DATABASE_DTC_DATABASE_ELEMENT_HPP_
