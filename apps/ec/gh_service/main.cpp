/**
 * @file gh_service.cpp
 * @author Grzegorz Horbaczewski (gregority9@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-11-30
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "apps/ec/gh_service/gh_app.hpp"
#include "ara/exec/adaptive_lifecycle.h"
int main(int argc, char const *argv[]){
    return ara::exec::RunAdaptiveLifecycle<srp::apps::MyGhApp>(argc, argv);
}