/**
 * @file dtc_database_element.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-12-23
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "mw/diag/dtc/database/dtc_database_element.hpp"

namespace simba {
namespace mw {
namespace dtc {

DtcDatabaseElement::DtcDatabaseElement(uint16_t dtc_error_code,
std::string details, uint8_t status)
    : dtc_error_code_{dtc_error_code}, details_{details}, status_{status} {
    }

void DtcDatabaseElement::SetDtcErrorCode(const uint16_t& dtc_error_code) {
    this->dtc_error_code_ = dtc_error_code;
}

void DtcDatabaseElement::SetDetails(const std::string& details) {
  this->details_ = details;
}
void DtcDatabaseElement::SetStatus(const uint8_t& status) {
    this->status_ = status;
}

uint16_t DtcDatabaseElement::GetDtcErrorCode()const {
    return this->dtc_error_code_;
}

std::string DtcDatabaseElement::GetDetails()const {
    return this->details_;
}
uint8_t DtcDatabaseElement::GetStatus() const {
    return this->status_;
}

}  // namespace dtc
}  // namespace mw
}  // namespace simba
