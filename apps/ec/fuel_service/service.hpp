/**
 * @file service.hpp
 * @author Wiktor Müller (wiktor.muller8@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2026-07-30
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#ifndef APPS_EC_FUEL_SERVICE_SERVICE_HPP_
#define APPS_EC_FUEL_SERVICE_SERVICE_HPP_

#include "srp/apps/FuelServiceSkeleton.h"

namespace srp {
namespace apps {
class MyFuelServiceSkeleton final : public srp::apps::FuelServiceSkeleton {
 private:
    std::function<bool(uint8_t, uint16_t)> on_start_refueling_cb_;
    std::function<bool(uint8_t)> on_abort_cb_;

 public:
    MyFuelServiceSkeleton(const ara::core::InstanceSpecifier& instance,
                      std::function<bool(uint8_t, uint16_t)> on_start_refueling,
                      std::function<bool(uint8_t)> on_abort);

    ara::core::Result<bool> StartRefueling(const RefuelRequestType& request) override;
    ara::core::Result<bool> Abort(const uint8_t& tank_id) override;
};

}  // namespace apps
}  // namespace srp


#endif  // APPS_EC_FUEL_SERVICE_SERVICE_HPP_
