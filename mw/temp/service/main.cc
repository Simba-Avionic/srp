/**
 * @file main.cc
 * @author Maciek Matuszewski (maciej.matuszewsky@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-03-02
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "mw/temp/service/temp_service.h"
#include "ara/exec/adaptive_lifecycle.h"

int main(int argc, char const *argv[]) {
  return ara::exec::RunAdaptiveLifecycle<srp::mw::temp::TempService>(argc,
                                                                     argv);
}
