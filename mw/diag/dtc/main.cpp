#include "core/application/application_factory.h"
#include "mw/diag/dtc/dtcService.hpp"

int main(int argc, char const *argv[]){
simba::core::ApplicationFactory<simba::mw::dtc::DtcService>::Start(argc,argv);
return 0;
}