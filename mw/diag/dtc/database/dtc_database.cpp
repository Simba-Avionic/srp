

#include "mw/diag/dtc/database/dtc_database.hpp"
#include <memory>
namespace simba{
namespace mw{
namespace dtc{

DtcDatabase::DtcDatabase(){
    this->active_errors=0;
    this->last_ID=0;
    //TODO init logger
}
uint16_t DtcDatabase::AddError(uint16_t service_id,uint16_t error_code,
                std::string details,DtcErrorStatus_t status){
    this->last_ID+=1;
    this->errors_.insert({this->last_ID,DtcDatabaseElement(service_id,error_code,details,status)});
    this->active_errors+=1;
    return this->last_ID;
}
uint16_t DtcDatabase::ManResetError(uint16_t error_id){
    auto error=this->errors_.find(error_id);
    if (error==this->errors_.end()){
        return 0;
    }
    error->second.SetStatus(DtcErrorStatus_t::kManReset);
    this->active_errors-=1;
    return error->first;
}
uint16_t DtcDatabase::AutoResetError(uint16_t error_id){
    auto error=this->errors_.find(error_id);
    if (error==this->errors_.end()){
        return 0;
    }
    this->active_errors-=1;
    error->second.SetStatus(DtcErrorStatus_t::kAutoReset);
    return error->first;
}
uint16_t DtcDatabase::ErrorNum(){
    return this->active_errors;
}
std::string DtcDatabase::GetErrorDetails(uint16_t error_id){
    auto error=this->errors_.find(error_id);
    if (error!=this->errors_.end()){
        return error->second.GetDetails();
    }
    return "";
}


}  // dtc
}  // mw
}  // simba