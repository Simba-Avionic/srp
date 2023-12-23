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

namespace simba{
namespace mw{
namespace dtc{

DtcDatabaseElement::DtcDatabaseElement(uint16_t service_id,std::string details,
            DtcErrorStatus_t status=DtcErrorStatus_t::kCreated)
    : service_id_{service_id},details_{details},status_{status}{
    }

void DtcDatabaseElement::SetService_id(const uint16_t& service_id){
    this->service_id_=service_id;
}

void DtcDatabaseElement::SetDetails(const std::string& details){
    this->details_=details;
}
void DtcDatabaseElement::SetStatus(const DtcErrorStatus_t& status){
    this->status_=status;
}

uint16_t DtcDatabaseElement::GetService_id(){
    return this->service_id_;
}

std::string DtcDatabaseElement::GetDetails(){
    return this->details_;
}
DtcErrorStatus_t DtcDatabaseElement::GetStatus(){
    return this->status_;
}

}
}
}