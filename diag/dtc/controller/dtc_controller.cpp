/**
 * @file dtc_controller.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-01-03
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "diag/dtc/controller/dtc_controller.hpp"
#include "diag/dtc/data/dtc_header.hpp"

#include <vector>

namespace simba {
namespace diag {
namespace dtc {

void DtcController::Init(
    std::shared_ptr<com::soc::SocketConfig> config) {
    this->sock_.Init(*config.get());
}

core::ErrorCode DtcController::RegisterError(const uint16_t &dtc_error_id,
   const std::vector<uint8_t> &payload, const uint8_t &dtc_status) {
    DtcHeader hdr{dtc_error_id, dtc_status};
    std::vector<uint8_t> data = this->factory_.GetBuffer(
            std::make_shared<DtcHeader>(hdr), payload);
    return this->sock_.Transmit("SIMBA.DIAG.DTC", 0, data);
}


}  // namespace dtc
}  // namespace diag
}  // namespace simba
