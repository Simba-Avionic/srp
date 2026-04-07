// Copyright 2025 Mikita Hapankou
#ifndef CORE_APOGEE_KALMANFILTERR7_H_
#define CORE_APOGEE_KALMANFILTERR7_H_

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

#endif // CORE_APOGEE_KALMANFILTERR7_H_
