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
#include "ara/diag/generic_data_identifier.h"

#include <vector>

#include "ara/diag/diag_error_domain.h"
#include "ara/diag/uds_error_domain.h"

namespace ara {
namespace diag {
namespace {
constexpr size_t kSubFunctionIndex{0};
constexpr size_t kDiDIdMsbIndex{1};
constexpr size_t kDiDIdIdLsbIndex{2};

constexpr uint8_t kReadSubFunction{0x22};
constexpr uint8_t kWriteSubFunction{0x2E};
}  // namespace

const uint8_t GenericDiD::cSid;

GenericDiD::GenericDiD(const core::InstanceSpecifier &specifier)
    : routing::RoutableUdsService(specifier, cSid) {}

ara::core::Result<OperationOutput> GenericDiD::HandleMessage(
    const std::vector<uint8_t> &requestData) {
  OperationOutput _response;

  const uint8_t _subFunction{requestData.at(kSubFunctionIndex)};
  auto _writeId{static_cast<uint16_t>(requestData.at(kDiDIdMsbIndex) << 8)};
  _writeId |= requestData.at(kDiDIdIdLsbIndex);
  std::vector<uint8_t> positive_response{
      static_cast<uint8_t>(_subFunction + 0x40), requestData.at(kDiDIdMsbIndex),
      requestData.at(kDiDIdIdLsbIndex)};
  switch (_subFunction) {
    case kReadSubFunction: {
      const auto res = Read();
      if (res.HasValue()) {
        auto &tem = res.Value().responseData;
        positive_response.insert(positive_response.end(), tem.begin(),
                                 tem.end());
        return OperationOutput{positive_response};
      }
      return res;
    }

    case kWriteSubFunction: {
      const std::vector<uint8_t> payload{requestData.begin() + 3,
                                         requestData.end()};
      const auto res = Write(payload);
      if (res.HasValue()) {
        return OperationOutput{positive_response};
      }
      return res.Error();
    }
    default:
      return ara::diag::MakeErrorCode(
          ara::diag::UdsDiagErrc::kSubFunctionNotSupported);
  }
  return ara::diag::MakeErrorCode(ara::diag::DiagErrc::kInvalidArgument, "");
}
}  // namespace diag
}  // namespace ara
