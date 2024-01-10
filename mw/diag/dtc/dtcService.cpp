/**
 * @file dtcService.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-12-23
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "mw/diag/dtc/dtcService.hpp"

namespace simba {
namespace mw {
namespace dtc {

DtcService::DtcService(const com::soc::SocketConfig& config) {
  this->sock_.Init(config);
  this->sock_.SetRXCallback(std::bind(RxCallback_));
}

}  // namespace dtc
}  // namespace mw
}  // namespace simba
