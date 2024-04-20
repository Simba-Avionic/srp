/**
 * @file test.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-04-20
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef APPS_TEST_TEST_H_
#define APPS_TEST_TEST_H_
#include <string>
#include <unordered_map>
#include "core/application/application_no_ipc.h"
namespace simba {
class Test : public core::ApplicationNoIPC {
 public:
    core::ErrorCode Run(std::stop_token token) final;
    core::ErrorCode Initialize(
      const std::unordered_map<std::string, std::string>& parms) final;
    ~Test() = default;
};

}  // namespace simba
#endif  // APPS_TEST_TEST_H_
