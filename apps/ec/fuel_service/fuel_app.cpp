/**
 * @file fuel_app.cpp
 * @author Wiktor Müller (wiktor.muller8@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2026-07-29
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#include "apps/ec/fuel_service/fuel_app.hpp"
#include "core/common/condition.h"
#include "ara/log/log.h"

namespace srp {
namespace apps {

namespace {
    constexpr auto kAppPathKey = "app_path";
    constexpr auto kConfigPath = "etc/config.json";

    constexpr auto kIpcInstance = "srp/apps/FuelService/FuelService_ipc";
    constexpr auto kUdpInstance = "srp/apps/FuelService/FuelService_udp";

    constexpr auto kServoInstance = "srp/apps/FuelService/ServoService";
    constexpr auto kEnvInstance = "srp/apps/FuelService/EnvApp";
    constexpr auto kRadioInstance = "srp/apps/FuelService/FcRadioServiceHandler.h";

    constexpr auto kMainLoopDelayMs = 1000;

    constexpr auto kOxidizer_Tank_ID = 0;
    constexpr auto kPFS_Tank_ID = 1;
}  // namespace

int FuelApp::Initialize(const std::map<ara::core::StringView, ara::core::StringView> parms) {
    ara::log::LogInfo() << "FuelApp: Starting initialization...";

    auto app_path_it = parms.find(kAppPathKey);
    if (app_path_it == parms.end()) {
        ara::log::LogFatal() << "FuelApp: app_path parameter not found!";
        return 1;
    }

    std::string config_path = std::string(app_path_it->second.data(), app_path_it->second.size()) + kConfigPath;

    auto tank_configs_opt = FuelConfigReader::LoadConfig(config_path);
    if (!tank_configs_opt.has_value() || tank_configs_opt.value().empty()) {
        ara::log::LogFatal() << "FuelApp: Failed to load valid tank configurations. Aborting initialization.";
        return 1;
    }

    auto on_start_refuelling = [this](uint8_t tank_id, uint16_t target_val) -> bool {
        if (controllers_.find(tank_id) != controllers_.end()) {
            controllers_[tank_id]->StartRefueling(configs_[tank_id].refueling_type, target_val);
            return true;
        }
        ara::log::LogError() << "Requested StartRefueling for unknown tank_id: " << tank_id;
        return false;
    };

    auto on_abort = [this](uint8_t tank_id) -> bool {
        if (controllers_.find(tank_id) != controllers_.end()) {
            controllers_[tank_id]->RequestAbort();
            return true;
        }
        ara::log::LogError() << "Requested Abort for unknown tank_id: " << tank_id;
        return false;
    };

    service_ipc_ = std::make_unique<MyFuelServiceSkeleton>(
        ara::core::InstanceSpecifier(kIpcInstance), on_start_refuelling, on_abort);
    service_udp_ = std::make_unique<MyFuelServiceSkeleton>(
        ara::core::InstanceSpecifier(kUdpInstance), on_start_refuelling, on_abort);

    for (const auto& cfg : tank_configs_opt.value()) {
        configs_[cfg.tank_id] = cfg;
        auto controller = std::make_shared<RefuelController>();
        controllers_[cfg.tank_id] = controller;
        BindControllerValves(cfg.tank_id, controller);
    }

    InitSomeIp();

    service_ipc_->StartOffer();
    service_udp_->StartOffer();

    ara::log::LogInfo() << "FuelApp: Initialization Complete";
    return 0;
}

void FuelApp::InitSomeIp() {
    servo_handler_ = nullptr;
    env_handler_ = nullptr;
    radio_handler_ = nullptr;

    servo_proxy_ = std::make_shared<ServoServiceProxy>(ara::core::InstanceSpecifier(kServoInstance));
    servo_proxy_->StartFindService([this](auto handler) {
        ara::log::LogInfo() << "FuelApp: ServoService discovered.";

        servo_handler_ = handler;
    });

    env_proxy_ = std::make_shared<env::EnvAppProxy>(ara::core::InstanceSpecifier(kEnvInstance));
    env_proxy_->StartFindService([this](auto handler) {
        ara::log::LogInfo() << "FuelApp: EnvApp discovered.";

        env_handler_ = handler;

        handler->newOxidizerPressEvent.Subscribe(1, [handler, this](uint8_t) {
            handler->newOxidizerPressEvent.SetReceiveHandler([handler, this]() {
                auto res = handler->newOxidizerPressEvent.GetNewSamples();
                if (res.HasValue() && controllers_.count(kOxidizer_Tank_ID)) {
                    controllers_[kOxidizer_Tank_ID]->OnTankPressureReceived(res.Value());
                }
            });
        });

        handler->newPressureFeedPressEvent.Subscribe(1, [handler, this](uint8_t) {
            handler->newPressureFeedPressEvent.SetReceiveHandler([handler, this]() {
                auto res = handler->newPressureFeedPressEvent.GetNewSamples();
                if (res.HasValue() && controllers_.count(kPFS_Tank_ID)) {
                    controllers_[kPFS_Tank_ID]->OnTankPressureReceived(res.Value());
                }
            });
        });
    });

    radio_proxy_ = std::make_shared<FcRadioServiceProxy>(ara::core::InstanceSpecifier(kRadioInstance));
    radio_proxy_->StartFindService([this](auto handler) {
        ara::log::LogInfo() << "FuelApp: RadioService discovered.";

        radio_handler_ = handler;

        handler->GSERocketMassEvent.Subscribe(1, [handler, this](uint8_t) {
            handler->GSERocketMassEvent.SetReceiveHandler([handler, this]() {
                auto res = handler->GSERocketMassEvent.GetNewSamples();
                if (res.HasValue()) {
                    for (auto& [id, ctrl] : controllers_) {
                        ctrl->OnRocketMassReceived(res.Value());
                    }
                }
            });
        });

        handler->GSEOxidizerTankPressEvent.Subscribe(1, [handler, this](uint8_t) {
            handler->GSEOxidizerTankPressEvent.SetReceiveHandler([handler, this]() {
                auto res = handler->GSEOxidizerTankPressEvent.GetNewSamples();
                if (res.HasValue() && controllers_.count(kOxidizer_Tank_ID)) {
                    controllers_[kOxidizer_Tank_ID]->OnGSPressureReceived(res.Value());
                }
            });
        });
    });
}

void FuelApp::BindControllerValves(uint8_t tank_id, std::shared_ptr<RefuelController> controller) {
    const auto& cfg = configs_[tank_id];

    SetValvePosCallback main_cb = [this, cfg](uint8_t pos) {
        if (radio_handler_) {
            if (cfg.tank_id == kOxidizer_Tank_ID) radio_handler_->SetGSEOxidizerFillValve(pos);
            else if (cfg.tank_id == kPFS_Tank_ID) radio_handler_->SetGSEPFSFillValve(pos);
        } else {
            ara::log::LogWarn() << "FuelApp: Main GS Valve command ignored - RadioService not connected.";
        }
    };

    SetValvePosCallback vent_cb = nullptr;

    if (cfg.rocket_vent_valve_id.has_value()) {
        vent_cb = [this, cfg](uint8_t pos) {
            if (servo_handler_) {
                if (cfg.tank_id == kOxidizer_Tank_ID) servo_handler_->SetOxidizerVentValve(pos);
            } else {
                ara::log::LogWarn() << "FuelApp: Vent Valve command ignored - ServoService not connected.";
            }
        };
    }

    SetValvePosCallback dump_cb = [this, cfg](uint8_t pos) {
        if (servo_handler_) {
            if (cfg.tank_id == kOxidizer_Tank_ID) servo_handler_->SetOxidizerDumpValve(pos);
            else if (cfg.tank_id == kPFS_Tank_ID) servo_handler_->SetPressureFeedVentValve(pos);
        } else {
            ara::log::LogWarn() << "FuelApp: Dump Valve command ignored - ServoService not connected.";
        }
    };

    controller->Initialize(cfg, main_cb, vent_cb, dump_cb);
}

int FuelApp::Run(const std::stop_token& token) {
    ara::log::LogInfo() << "FuelApp: Running Main Loop";

    while (!token.stop_requested()) {
        core::condition::wait_for(std::chrono::milliseconds(kMainLoopDelayMs), token);
        // @todo: Implement state event broadcasting
    }

    ara::log::LogInfo() << "FuelApp: Run complete, closing";
    return 0;
}

}  // namespace apps
}  // namespace srp
