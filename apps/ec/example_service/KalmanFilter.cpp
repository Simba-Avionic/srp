#include "KalmanFilter.h"


KalmanFilter::KalmanFilter(const Matrix &F, const Matrix &B, const Matrix &H, const Matrix &Q, const Matrix &R, const Matrix &P, const Vector &x)
: F(F), B(B), H(H), Q(Q), R(R), P(P), x(x) {}


void KalmanFilter::predict(const Vector &u)
{
    x = F * x + B * u;
    P = F * P * F.transpose() + Q;
}

void KalmanFilter::update(const Vector &z)
{
    Vector y = z - H * x;
    Matrix S = H * P * H.transpose() + R;
    Matrix K = P * H.transpose() * S.inverse();
    x = x + K * y;
    Matrix I = Matrix::Identity(P.rows(), P.cols());
    P = (I - K * H) * P;
}

Eigen::VectorXd KalmanFilter::getState() const
{
    return x;
}


