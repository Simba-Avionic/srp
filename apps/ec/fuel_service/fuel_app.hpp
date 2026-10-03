/**
 * @file fuel_app.hpp
 * @author Wiktor Müller (wiktor.muller8@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2026-07-29
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#ifndef APPS_EC_FUEL_SERVICE_FUEL_APP_HPP_
#define APPS_EC_FUEL_SERVICE_FUEL_APP_HPP_

#include <string>
#include <unordered_map>
#include <vector>
#include <map>
#include <memory>

#include "ara/exec/adaptive_application.h"

#include "apps/ec/fuel_service/service.hpp"
#include "apps/ec/fuel_service/controller/refuel_controller.hpp"
#include "apps/ec/fuel_service/fuel_config_reader.hpp"

#include "srp/apps/ServoService/ServoServiceHandler.h"
#include "srp/env/EnvApp/EnvAppHandler.h"
#include "srp/apps/FcRadioService/FcRadioServiceHandler.h"

namespace srp {
namespace apps {
class FuelApp final : public ara::exec::AdaptiveApplication {
 private:
    std::unordered_map<uint8_t, std::shared_ptr<RefuelController>> controllers_;
    std::unordered_map<uint8_t, TankConfig> configs_;

    std::unique_ptr<MyFuelServiceSkeleton> service_ipc_;
    std::unique_ptr<MyFuelServiceSkeleton> service_udp_;

    std::shared_ptr<ServoServiceProxy> servo_proxy_;
    std::shared_ptr<env::EnvAppProxy> env_proxy_;
    std::shared_ptr<FcRadioServiceProxy> radio_proxy_;

    std::shared_ptr<ServoServiceHandler> servo_handler_;
    std::shared_ptr<env::EnvAppHandler> env_handler_;
    std::shared_ptr<FcRadioServiceHandler> radio_handler_;

    void InitSomeIp();
    void BindControllerValves(uint8_t tank_id, std::shared_ptr<RefuelController> controller);

 protected:
    int Run(const std::stop_token& token) override;
    int Initialize(const std::map<ara::core::StringView, ara::core::StringView> parms) override;

 public:
    FuelApp() = default;
};

}  // namespace apps
}  // namespace srp


#endif  // APPS_EC_FUEL_SERVICE_FUEL_APP_HPP_
