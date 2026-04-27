/**
 * @file main.cpp
 * @author Krzysztof Kondracki (kondracki.christopher@gmail.com)
 * @brief
 * @version 0.1
 * @date 2026-04-13
 *
 * @copyright Copyright (c) 2026
 *
 */
#include "apps/fc/logger_service/service/logger_service.hpp"
#include "ara/exec/adaptive_lifecycle.h"

int main(int argc, char const* argv[]) {
  return ara::exec::RunAdaptiveLifecycle<srp::logger::LoggerService>(argc, argv);
}
