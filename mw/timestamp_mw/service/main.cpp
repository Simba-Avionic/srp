/**
 * @file main.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-02-12
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "mw/timestamp_mw/service/timestamp_service.hpp"
#include "ara/exec/adaptive_lifecycle.h"

int main(int argc, char const *argv[]) {
return ara::exec::RunAdaptiveLifecycle<srp::mw::TimestampService>(argc,
                                                                    argv);
}
