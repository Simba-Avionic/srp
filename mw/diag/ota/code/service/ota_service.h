/**
 * @file ota_service.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-01-05
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef MW_DIAG_OTA_CODE_SERVICE_OTA_SERVICE_H_
#define MW_DIAG_OTA_CODE_SERVICE_OTA_SERVICE_H_
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include "diag/base/controller/idiag_controller.h"
#include "core/application/application_mw.h"
#include "core/application/parm.h"
namespace simba {
namespace mw {
namespace ota {
class OtaService : public core::ApplicationMW {
 private:
  std::unique_ptr<diag::IDiagController> diag_controller;
 public:
    void Run(const std::unordered_map<std::string, core::Parm>& parms) override;
    OtaService();
};
}  // namespace ota
}  // namespace mw
}  // namespace simba

#endif  // MW_DIAG_OTA_CODE_SERVICE_OTA_SERVICE_H_
