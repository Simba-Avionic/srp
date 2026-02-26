/**
 * @file env_service.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-04-04
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <memory>
#include <map>
#include <string>
#include <utility>
#include <sstream>
#include <iomanip>
#include "apps/ec/system_stat_service/system_stat_service.hpp"
#include "core/common/condition.h"
#include "ara/log/log.h"
#include "core/sys/system_stat.hpp"

namespace srp {
namespace sysService {

namespace {
    constexpr auto kIpcInstanceSpecifier = "srp/apps/SysStatService/SysStatService_ipc";
    constexpr auto kUdpInstanceSpecifier = "srp/apps/SysStatService/SysStatService_udp";
    constexpr auto kDelay = 1000;
}

SystemStatService::SystemStatService():
                service_ipc{ara::core::InstanceSpecifier{kIpcInstanceSpecifier}},
                service_udp{ara::core::InstanceSpecifier{kUdpInstanceSpecifier}} {
}


int SystemStatService::Initialize(const std::map<ara::core::StringView, ara::core::StringView>
                      parms) {
    service_ipc.StartOffer();
    service_udp.StartOffer();
    return 0;
}


int SystemStatService::Run(const std::stop_token& token) {
    while (!token.stop_requested()) {
        apps::SysStatType stats;
        stats.cpu_usage = static_cast<float>(core::stat::SystemStats::get_cpu_usage());
        stats.mem_usage = core::stat::SystemStats::get_ram_usage();
        stats.disk_utilization = static_cast<float>(core::stat::SystemStats::get_disk_space());
        service_ipc.NewSystemUsage.Update(stats);
        service_udp.NewSystemUsage.Update(stats);
        core::condition::wait_for(std::chrono::milliseconds(kDelay), token);
    }
    service_ipc.StopOffer();
    service_udp.StopOffer();
    return 0;
}

}  // namespace sysService
}  // namespace srp
