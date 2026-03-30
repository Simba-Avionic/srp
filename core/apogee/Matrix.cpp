#include "Matrix.h"
#include <memory>
#include <string>
#include <stdexcept>

Matrix::Matrix(std::initializer_list<std::initializer_list<float>> list){
    rows = list.size();
    columns = list.begin()->size();
    for(const auto& row : list){
        if(row.size() != columns){
             throw std::invalid_argument("All rows in the matrix must have the same number of columns.");
        }
    }

    array = std::make_unique<std::unique_ptr<float[]>[]>(rows);
    for (size_t i = 0; i < rows; ++i) {
        array[i] = std::make_unique<float[]>(columns);
    }

    size_t r = 0;
    for (const auto& row : list) {
        size_t c = 0;
        for (double val : row) {
            array[r][c] = val;
            ++c;
        }
        ++r;
    }
}

Matrix::Matrix(size_t rows, size_t columns)
{
    array = std::make_unique<std::unique_ptr<float[]>[]>(rows);
    for (size_t r = 0; r < rows; ++r) {
        array[r] = std::make_unique<float[]>(columns);
        for(size_t c = 0; c < columns; ++c){
            (*this)(r,c) = 0;
        }
    }
    this->rows = rows;
    this->columns = columns;
}

Matrix::Matrix(const Matrix &other) : rows(other.getRows()), columns(other.getColumns())
{
    array = std::make_unique<std::unique_ptr<float[]>[]>(rows);
    for (size_t r = 0; r < rows; ++r) {
        array[r] = std::make_unique<float[]>(columns);
        for(size_t c = 0; c < columns; ++c){
            (*this)(r,c) = other(r,c);
        }
    }
}

Matrix Matrix::Identity(size_t size)
{
    Matrix m(size, size);
    for (size_t i = 0; i < size; i++){
        m(i,i)= 1;
    } 

    return m;
}

float& Matrix::operator ()(size_t r, size_t c) {
    return array[r][c];
}

const float &Matrix::operator()(size_t r, size_t c) const
{
    return array[r][c];
}

Matrix Matrix::operator+(const Matrix &other) const
{
    if(rows != other.rows || columns != other.columns) 
        throw std::invalid_argument("Cannot add matrices: both matrices must have the same dimensions.");
    Matrix result(rows, columns);

    for(size_t r = 0; r < rows; ++r){
        for(size_t c = 0; c < columns; ++c){
            result(r,c) = (*this)(r,c) + other(r,c);
        }
    }

    return result;
}

Matrix Matrix::operator*(const Matrix &other) const
{
    if(columns != other.rows)
        throw std::invalid_argument("Cannot multiply matrices: the number of columns in the first matrix must equal the number of rows in the second matrix.");
    Matrix result(rows, other.columns);

    for (size_t r = 0; r < rows; ++r) {
        for (size_t c = 0; c < other.columns; ++c) {
            float sum = 0;
            for (size_t k = 0; k < columns; ++k) {
                sum += (*this)(r, k) * other(k, c);
            }
            result(r, c) = sum;
        }
    }

    return result;
}

Matrix Matrix::operator-(const Matrix &other) const
{
     if(rows != other.rows || columns != other.columns) 
        throw std::invalid_argument("Cannot substract matrices: both matrices must have the same dimensions.");
    Matrix result(rows, columns);

    for(size_t r = 0; r < rows; ++r){
        for(size_t c = 0; c < columns; ++c){
            result(r,c) = (*this)(r,c) - other(r,c);
        }
    }

    return result;
}

Matrix &Matrix::operator=(Matrix &&other)
{
    rows = other.getRows();
    columns = other.getColumns();
    array = std::move(other.array);
    return *this;
}

Matrix Matrix::transpose() const
{
    Matrix result(columns, rows);

    for(size_t r = 0; r < rows; r++){
        for(size_t c = 0; c < columns; c++){
           result(c,r) = (*this)(r,c);
        }
    }

    return result;
}

Matrix Matrix::inverse1x1() const
{
    Matrix result(1, 1);
    result(0,0) = 1 / (*this)(0,0);
    return result;
}



size_t Matrix::getRows() const
{
    return rows;
}

size_t Matrix::getColumns() const
{
    return columns;
}

std::string Matrix::toString() const
{
    std::string str = "";
    for(size_t r=0; r < rows; r++){
        str += "|";
        for(size_t c=0; c < columns; c++){
            str += std::to_string(array[r][c]);
            if(c!=columns-1) str += ", ";
        }
        str += "|\n";
    }


    return str;
}
