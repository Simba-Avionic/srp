/**
 * @file main.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-01-05
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "core/application/application_factory.h"
#include "mw/diag/ota/code/service/ota_service.h"
int main(int argc, char const *argv[])
{
    simba::core::ApplicationFactory<simba::mw::ota::OtaService>::Start(argc,argv);
    return 0;
}
