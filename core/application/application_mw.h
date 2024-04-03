/**
 * @file application_mw.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief Interface for all Simba mw application
 * @version 1.0
 * @date 2023-10-23
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef CORE_APPLICATION_APPLICATION_MW_H_
#define CORE_APPLICATION_APPLICATION_MW_H_
#include <memory>
#include <string>
#include <unordered_map>

#include "core/application/application_common.h"
#include "core/common/error_code.h"
#include "diag/base/controller/diag_controller.h"
namespace simba {
namespace core {
class ApplicationMW : public ApplicationCommon {
 protected:
  std::unique_ptr<diag::DiagController> diag_controller;
  void onRun(const std::unordered_map<std::string, std::string>& parms) final;
  ErrorCode MwConfig(const std::unordered_map<std::string, std::string>& parms);

 public:
  virtual ~ApplicationMW() = default;
};
}  // namespace core
}  // namespace simba
#endif  // CORE_APPLICATION_APPLICATION_MW_H_
