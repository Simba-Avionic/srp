#include <cmath>
#include "KalmanFilterR7.h"

KalmanFilterR7::KalmanFilterR7(float dt) : KalmanFilter(
    Matrix{{1.0f, dt}, {0.0f, 1.0f}},        // F - macierz przejścia stanu
    Matrix{{0.0f}, {dt}},                    // B - macierz sterowania
    Matrix{{1.0f, 0.0f}},                    // H - macierz obserwacji
    Matrix{{0.01f, 0.0f}, {0.0f, 0.01f}},    // Q - szum procesu
    Matrix{{19.0f}},                         // R - szum pomiaru
    Matrix::Identity(2),                     // P - kowariancja stanu (Identity na 1 argumencie)
    Matrix{{0.0f}, {0.0f}}                   // x - wektor stanu (pozycja, prędkość)
) {}

Matrix KalmanFilterR7::processMeasurement(float acceleration, float pressure)
{
    Matrix u(1, 1);
    u(0,0) = acceleration;
    predict(u);

    Matrix z(1,1);
    z(0, 0) = static_cast<float>(pressureToAltitude(pressure));
    update(z);
    return getState();
}


 double KalmanFilterR7::pressureToAltitude(double p) {
    const double p0 = 101325.0,  // standardowe ciśnienie na poziomie morza (Pa)
                    T = 288.15,     // standardowa temperatura na poziomie morza (Kelwiny)
                    L = 0.0065,     // spadek temp. na 1 metr, K/m
                    R = 8.31447,    // uniwersalna stała gazowa, J/(mol·K)
                    g = 9.80665,    // przyspieszenie ziemskie, m/s^2
                    M = 0.0289644;  // molowa masa powietrza, kg/mol

    // h = (T / L) * (1 - (p / p0) ^ ((R * L) / (g * M)))
    return (T / L) * (1 - pow(p / p0, (R * L) / (g * M)));
}