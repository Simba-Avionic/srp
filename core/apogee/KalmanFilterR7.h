#ifndef Kalman_Filter_R7
#define Kalman_Filter_R7

#include "KalmanFilter.h"

class KalmanFilterR7 : public KalmanFilter {
 public:
  explicit KalmanFilterR7(float dt);
  Matrix processMeasurement(float acceleration, float pressure);

 private:
  double pressureToAltitude(double p);
  using KalmanFilter::predict;
  using KalmanFilter::update;
};

#endif  