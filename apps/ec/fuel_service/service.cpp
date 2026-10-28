/**
 * @file service.cpp
 * @author Wiktor Müller (wiktor.muller8@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2026-07-30
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#include <utility>
#include "apps/ec/fuel_service/service.hpp"

namespace srp {
namespace apps {

MyFuelServiceSkeleton::MyFuelServiceSkeleton(const ara::core::InstanceSpecifier& instance,
                                    std::function<bool(uint8_t, uint16_t)> on_start_refueling,
                                    std::function<bool(uint8_t)> on_abort)
: srp::apps::FuelServiceSkeleton(instance), on_start_refueling_cb_(std::move(on_start_refueling)),
        on_abort_cb_(std::move(on_abort)) {}

ara::core::Result<bool> MyFuelServiceSkeleton::StartRefueling(const RefuelRequestType& request) {
    return on_start_refueling_cb_(request.tank_id, request.target_value);
}

ara::core::Result<bool> MyFuelServiceSkeleton::Abort(const uint8_t& tank_id) {
    return on_abort_cb_(tank_id);
}

}  // namespace apps
}  // namespace srp
