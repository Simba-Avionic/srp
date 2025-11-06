#include "KalmanFilterR7.h"

KalmanFilterR7::KalmanFilterR7(float dt):KalmanFilter(
    makeMatrix({{1, dt}, {0, 1}}),      // F - macierz przejścia stanu
    makeMatrix({{0},{dt}}),             // B - macierz sterowania
    makeMatrix({{1, 0}}),               // H - macierz obserwacji
    makeMatrix({{0.01, 0}, {0, 0.01}}), // Q - szum procesu
    makeMatrix({{19}}),                 // R - szum pomiaru
    Eigen::MatrixXd::Identity(2, 2),    // P - kowariancja stanu
    makeMatrix({{0},{0}})               // x - wektor stanu (pozycja, prędkość)
    ){}

Eigen::Vector2d KalmanFilterR7::processMeasurement(float acceleration, float pressure)
{
    Eigen::VectorXd u(1);
    u(0) = acceleration;
    predict(u);

    Eigen::VectorXd z(1);
    z(0) = pressureToAltitude(pressure);
    update(z);
    return getState();
}

Eigen::MatrixXd KalmanFilterR7::makeMatrix(std::initializer_list<std::initializer_list<double>> list) {
    size_t rows = list.size();
    size_t cols = list.begin()->size();

    Eigen::MatrixXd m(rows, cols);

    size_t r = 0;
    for (const auto& row : list) {
        size_t c = 0;
        for (double val : row) {
            m(r, c) = val;
            ++c;
        }
        ++r;
    }
    return m;
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