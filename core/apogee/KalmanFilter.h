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
#ifndef CORE_APOGEE_KALMANFILTER_H_
#define CORE_APOGEE_KALMANFILTER_H_

#include "Matrix.h"

class KalmanFilter {
 public:
  using Vector = Matrix;

  KalmanFilter(const Matrix& F, const Matrix& B, const Matrix& H,
               const Matrix& Q, const Matrix& R, const Matrix& P,
               const Vector& x);

  void predict(const Vector& u);

  void update(const Vector& z);
  Vector getState() const;

 protected:
  Matrix F;  // macierz przejścia stanu
  Matrix B;  // macierz sterowania
  Matrix H;  // macierz obserwacji
  Matrix Q;  // szum procesu
  Matrix R;  // szum pomiaru
  Matrix P;  // kowariancja stanu
  Vector x;  // wektor stanu
};

#endif  // CORE_APOGEE_KALMANFILTER_H_
