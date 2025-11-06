#include <Eigen/Dense>

class KalmanFilter {
public:
    using Matrix = Eigen::MatrixXd;
    using Vector = Eigen::VectorXd;

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
