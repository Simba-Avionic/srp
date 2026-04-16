/**
 * @file KalmanFilter.cpp
 * @author Mikita Hapankou
 * @brief 
 * @version 0.1
 * @date 2026-04-08
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#ifndef CORE_APOGEE_KALMANFILTERR7_H_
#define CORE_APOGEE_KALMANFILTERR7_H_

#include "KalmanFilter.h"

class KalmanFilterR7 : public KalmanFilter {
 public:
  explicit KalmanFilterR7(const float& dt);
  Matrix processMeasurement(const float& acceleration, const float& pressure);

 private:
  double pressureToAltitude(double p);
  using KalmanFilter::predict;
  using KalmanFilter::update;
};

#endif  // CORE_APOGEE_KALMANFILTERR7_H_
