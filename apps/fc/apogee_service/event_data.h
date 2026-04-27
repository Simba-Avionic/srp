/**
 * @file event_data.h
 * @author Wiktor Laska
 * @version 0.1
 * @date 2026-04-25
 */
#ifndef EVENT_DATA_H_
#define EVENT_DATA_H_

#include <shared_mutex>
#include <mutex>
#include <memory>
#include <functional>

namespace srp {
namespace apps {

template <typename T>
class ThreadSafe {
 public:
    void set(const T& value) {
        std::unique_lock lock(mtx_);
        data_ = value;
    }
    
    T get() const {
        std::shared_lock lock(mtx_);
        return data_;
    }
    
 private:
    mutable std::shared_mutex mtx_; 
    T data_{};
};

class ApogeeEventData {
 private:
    ThreadSafe<float> pressure_;         // Ciśnienie
    ThreadSafe<float> accel_z_;          // Przyspieszenie (oś Z)
    ThreadSafe<bool> apogee_detected_;   // Czy apogeum zostało wykryte

 public:
    static std::shared_ptr<ApogeeEventData> GetInstance();
    float GetPressure();
    void SetPressure(float val);

    float GetAccelZ();
    void SetAccelZ(float val);

    bool IsApogeeDetected();
    void SetApogeeDetected(bool val);
};

}  // namespace apps
}  // namespace srp

#endif  //  EVENT_DATA_H_