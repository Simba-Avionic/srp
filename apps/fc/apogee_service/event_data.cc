#include "event_data.h"

namespace srp {
namespace apps {

namespace {
    static std::shared_ptr<ApogeeEventData> instance = nullptr;
}

std::shared_ptr<ApogeeEventData> ApogeeEventData::GetInstance() {
    if (instance == nullptr) {
        instance = std::make_shared<ApogeeEventData>();
    }
    return instance;
}

float ApogeeEventData::GetPressure() { return pressure_.get(); }
void ApogeeEventData::SetPressure(float val) { pressure_.set(val); }

float ApogeeEventData::GetAccelZ() { return accel_z_.get(); }
void ApogeeEventData::SetAccelZ(float val) { accel_z_.set(val); }

bool ApogeeEventData::IsApogeeDetected() { return apogee_detected_.get(); }
void ApogeeEventData::SetApogeeDetected(bool val) { apogee_detected_.set(val); }

}  // namespace apps
}  // namespace srp