#pragma once
#include <initializer_list>
#include <memory>
#include <string>

class Matrix {
private:
    size_t rows = 0;
    size_t columns = 0;
    std::unique_ptr<std::unique_ptr<float[]>[]> array;
public:
    Matrix(std::initializer_list<std::initializer_list<float>> list);
    Matrix(size_t rows, size_t columns);
    Matrix(const Matrix& other);

    static Matrix Identity(size_t size);

    float& operator ()(size_t r, size_t c);
    const float& operator ()(size_t r, size_t c) const;
    Matrix operator +(const Matrix& other) const;
    Matrix operator *(const Matrix& other) const;
    Matrix operator -(const Matrix& other) const;
    Matrix& operator=(Matrix&&);
    Matrix transpose() const;
    Matrix inverse1x1() const;
    size_t getRows() const;
    size_t getColumns() const;
    std::string toString() const;
};