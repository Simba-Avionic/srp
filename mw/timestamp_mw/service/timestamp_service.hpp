/**
 * @file timestamp_service.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-02-12
 * 
 * @copyright Copyright (c) 2025
 * 
 */


#ifndef MW_TIMESTAMP_MW_SERVICE_TIMESTAMP_SERVICE_HPP_
#define MW_TIMESTAMP_MW_SERVICE_TIMESTAMP_SERVICE_HPP_
#include <map>
#include <memory>
#include <thread>  // NOLINT
#include "ara/log/log.h"
#include "ara/exec/adaptive_application.h"
#include "core/timestamp/timestamp_driver.hpp"
#include "mw/timestamp_mw/ntp/controller/ntp_controller.hpp"

namespace srp {
namespace mw {

class TimestampService final : public ara::exec::AdaptiveApplication {
 private:
    srp::tinyNTP::NtpController ntp_controller;
    /**
     * @brief This function is called to launch the application
     *
     * @param token stop token
     */
    int Run(const std::stop_token& token) override;
    /**
     * @brief This function is called to initialize the application
     *
     * @param parms map with params
     */
    int Initialize(const std::map<ara::core::StringView, ara::core::StringView>
                        parms) override;
 public:
};

}  // namespace mw
}  // namespace srp

#endif  // MW_TIMESTAMP_MW_SERVICE_TIMESTAMP_SERVICE_HPP_
