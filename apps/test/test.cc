/**
 * @file test.cc
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-04-20
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "apps/test/test.h"

#include "core/logger/Logger.h"
namespace simba {
core::ErrorCode Test::Run(std::stop_token token) {
    AppLogger::Info("Hello World");
    return core::ErrorCode::kOk;
}

core::ErrorCode Test::Initialize(
    const std::unordered_map<std::string, std::string>& parms) {
  return core::ErrorCode::kOk;
}

}  // namespace simba
