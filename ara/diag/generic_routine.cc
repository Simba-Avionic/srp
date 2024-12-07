/**
 * @file generic_routine.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-11-30
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "ara/diag/generic_routine.h"

#include <vector>

#include "ara/diag/diag_error_domain.h"
#include "ara/diag/uds_error_domain.h"

namespace ara {
namespace diag {
const uint8_t GenericRoutine::cSid;

GenericRoutine::GenericRoutine(const core::InstanceSpecifier &specifier)
    : routing::RoutableUdsService(specifier, cSid) {}

ara::core::Result<OperationOutput> GenericRoutine::HandleMessage(
    const std::vector<uint8_t> &requestData) {
  const size_t cSubFunctionIndex{1};
  const size_t cRoutineIdMsbIndex{2};
  const size_t cRoutineIdLsbIndex{3};

  const uint8_t cStartSubFunction{0x01};
  const uint8_t cStopSubFunction{0x02};
  const uint8_t cResultRequestSubFunction{0x03};

  OperationOutput _response;

  if (requestData.size() > cRoutineIdLsbIndex) {
    uint8_t _subFunction{requestData.at(cSubFunctionIndex)};

    auto _routineId{
        static_cast<uint16_t>(requestData.at(cRoutineIdMsbIndex) << 8)};
    _routineId |= requestData.at(cRoutineIdLsbIndex);
    std::vector<uint8_t> positive_response{cSid, _subFunction,
                                           requestData.at(cRoutineIdMsbIndex),
                                           requestData.at(cRoutineIdLsbIndex)};
    switch (_subFunction) {
      case cStartSubFunction: {
        const std::vector<uint8_t> payload{requestData.begin() + 4,
                                           requestData.end()};
        const auto res = Start(payload);
        if (res.HasValue()) {
          auto &tem = res.Value().responseData;
          positive_response.insert(positive_response.end(), tem.begin(),
                                   tem.end());
          return OperationOutput{positive_response};
        }
        return res;
      }

      case cStopSubFunction: {
        const std::vector<uint8_t> payload{requestData.begin() + 4,
                                           requestData.end()};
        const auto res = Stop(payload);
        if (res.HasValue()) {
          auto &tem = res.Value().responseData;
          positive_response.insert(positive_response.end(), tem.begin(),
                                   tem.end());
          return OperationOutput{positive_response};
        }
        return res;
      }

      case cResultRequestSubFunction: {
        const std::vector<uint8_t> payload{requestData.begin() + 4,
                                           requestData.end()};
        const auto res = RequestResults(payload);
        if (res.HasValue()) {
          auto &tem = res.Value().responseData;
          positive_response.insert(positive_response.end(), tem.begin(),
                                   tem.end());
          return OperationOutput{positive_response};
        }
        return res;
      }

      default:
        return ara::diag::MakeErrorCode(
            ara::diag::UdsDiagErrc::kSubFunctionNotSupported);
    }
  } else {
    return ara::diag::MakeErrorCode(
        ara ::diag::UdsDiagErrc::kInvalidMessageLengthFormat);
  }

  return ara::diag::MakeErrorCode(ara::diag::DiagErrc::kInvalidArgument, "");
}
}  // namespace diag
}  // namespace ara
