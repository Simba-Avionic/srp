/**
 * @file dtcService.h
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-12-23
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef MW_DIAG_DTC_SERVICE_DTCSERVICE_H_
#define MW_DIAG_DTC_SERVICE_DTCSERVICE_H_

#include <inttypes.h>
#include <stdio.h>

#include <fstream>
#include <memory>
#include <stop_token>
#include <string>
#include <unordered_map>
#include <vector>

#include "core/application/application_mw.h"
#include "core/logger/Logger.h"
#include "mw/diag/dtc/service/database.h"
#include "mw/diag/dtc/service/diag/dtc_by_mask_job.h"
#include "mw/diag/dtc/service/diag/dtc_list_job.h"
#include "mw/diag/dtc/service/diag/dtc_snapshot_job.h"
#include "mw/diag/dtc/service/diag/clear_dtc_job.h"
namespace simba {
namespace mw {
namespace dtc {

class DtcService : public core::ApplicationMW {
 protected:
  std::unique_ptr<DtcListJob> dtc_list_;
  std::unique_ptr<DtcByMaskJob> dtc_by_mask_job_;
  std::unique_ptr<DtcSnapshotJob> dtc_snapshot_job_;
  std::unique_ptr<ClearDtcJob> clear_dtc_job_;
  std::shared_ptr<DataBase> db_;

  void LoadConfig(const std::string& path);
    
 public:
  DtcService();
  /**
   * @brief This function is called to launch the application
   *
   * @param token stop token
   */
  core::ErrorCode Run(std::stop_token token) final;
  /**
   * @brief This function is called to initialize the application
   *
   * @param parms map with parms
   */
  core::ErrorCode Initialize(
      const std::unordered_map<std::string, std::string>& parms) final;
};

}  // namespace dtc
}  // namespace mw
}  // namespace simba

#endif // MW_DIAG_DTC_SERVICE_DTCSERVICE_H_
