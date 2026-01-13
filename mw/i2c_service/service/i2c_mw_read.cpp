#include "mw/i2c_service/service/i2c_mw_read.h"
#include "mw/i2c_service/service/i2c_service.h" 

namespace srp {
namespace mw {

I2CMWREAD::I2CMWREAD(const ara::core::InstanceSpecifier &specifier, I2CService* service, std::shared_ptr<
        core::i2c::II2CDriver> i2c_)
    : ara::diag::GenericDiD{specifier}, i2c_service_(service), i2c_(i2c_) {}

ara::core::Result<void> I2CMWREAD::Write(const std::vector<uint8_t> &payload) noexcept {
    if (payload.size() < 3 || !payload.size()%2) {
        ara::log::LogInfo() << "Wrong payload size: " << payload.size();
        return ara::diag::MakeErrorCode(ara::diag::UdsDiagErrc::kSubFunctionNotSupported);
    }
    if (this->i2c_->Ioctl(payload[0]) != core::ErrorCode::kOk) {
        ara::log::LogInfo() << "Wrong Input for payload i2c read";
      return {};
    }

    ara::log::LogInfo() << "Receive diag write req, calling I2CService::WriteRead";
    std::vector<uint8_t> vec(payload.begin() + 1, payload.end());
    auto result = i2c_service_->WriteRead(vec, nullptr);

    if (result == std::nullopt) {
        ara::log::LogInfo() << "Wrong Input for i2c WriteRead";
        return ara::diag::MakeErrorCode(ara::diag::UdsDiagErrc::kConditionsNotCorrect);
    }

    return {};
}

} // namespace mw
} // namespace srp