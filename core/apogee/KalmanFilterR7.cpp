/**
 * @file KalmanFilter.cpp
 * @author Mikita Hapankou
 * @brief 
 * @version 0.1
 * @date 2026-04-08
 * * @copyright Copyright (c) 2026
 * */
#include <cmath>
#include "KalmanFilterR7.h"

KalmanFilterR7::KalmanFilterR7(const float& dt) : KalmanFilter(
    Matrix{{1.0f, dt}, {0.0f, 1.0f}},        // F - macierz przejścia stanu
    Matrix{{0.0f}, {dt}},                    // B - macierz sterowania
    Matrix{{1.0f, 0.0f}},                    // H - macierz obserwacji
    Matrix{{0.01f, 0.0f}, {0.0f, 0.01f}},    // Q - szum procesu
    Matrix{{19.0f}},                         // R - szum pomiaru
    Matrix::Identity(2),                     // P - kowariancja stanu (Identity na 1 argumencie)
    Matrix{{0.0f}, {0.0f}}                   // x - wektor stanu (pozycja, prędkość)
) {}

Matrix KalmanFilterR7::processMeasurement(const float& acceleration, const float& pressure) {
    Matrix u(1, 1);
    u(0, 0) = acceleration;
    predict(u);

    Matrix z(1, 1);
    z(0, 0) = static_cast<float>(pressureToAltitude(static_cast<double>(pressure)));
    update(z);
    return getState();
}

namespace {
    static constexpr auto kP0 = 101325.0;  // standardowe ciśnienie na poziomie morza (Pa)
    static constexpr auto kT = 288.15;    // standardowa temperatura na poziomie morza (Kelwiny)
    static constexpr auto kL = 0.0065;    // spadek temp. na 1 metr, K/m
    static constexpr auto kR = 8.31447;    // uniwersalna stała gazowa, J/(mol·K)
    static constexpr auto kg = 9.80665;    // przyspieszenie ziemskie, m/s^2
    static constexpr auto kM = 0.0289644;  // molowa masa powietrza, kg/mol
    consteval double getPressurePow() {
        return (kR * kL) / (kg * kM);
    }
}  // namespace

double KalmanFilterR7::pressureToAltitude(double p) {
    return (kT / kL) * (1 - pow(p / kP0, getPressurePow()));
}
