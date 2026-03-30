#include "KalmanFilter.h"

class KalmanFilterR7 : public KalmanFilter {

public:
    KalmanFilterR7(float dt);  
    Matrix processMeasurement(float acceleration, float pressure);
private:
    double pressureToAltitude(double p);
    using KalmanFilter::predict;
    using KalmanFilter::update; 
};
