#pragma once
#include <vector>
#include "Vector2D.h"
#include <stdexcept>

class Matrix {
protected:
	int _rows, _cols;
	std::vector<std::vector<double>> _data;
public:
	Matrix() : _cols(0), _rows(0) {}
	Matrix(int cols, int rows) : _cols(cols), _rows(rows) {
		_data.resize(rows, std::vector<double>(cols, 0.0));
	}

	Vector2D GetSize() const {
		return Vector2D(_cols, _rows);
	}

	bool CanCalculate(const Matrix& other) const {
		return GetSize() == other.GetSize();
	}

	Matrix operator+(const Matrix& other) {
		if (not CanCalculate(other)) {
			throw std::invalid_argument("Matrix sizes do not match for addition.");
		}
		Matrix result(_cols, _rows);
		for (int i = 0; i < _rows; ++i) {
			for (int j = 0; j < _cols; ++j) {
				result._data[i][j] = _data[i][j] + other._data[i][j];
			}
		}
		return result;
	}

	Matrix operator-(const Matrix& other) {

		Matrix result(_cols, _rows);
		for (int i = 0; i < _rows; ++i) {
			for (int j = 0; j < _cols; ++j) {
				result._data[i][j] = _data[i][j] - other._data[i][j];
			}
		}
		return result;
	}

	Matrix operator*(const Matrix& other) {
		if (not CanCalculate(other)) {
			throw std::invalid_argument("Matrix sizes do not match for addition.");
		}
		Matrix result(other._cols, _rows);
		for (int i = 0; i < _rows; ++i) {
			for (int j = 0; j < other._cols; ++j) {
				for (int k = 0; k < _cols; ++k) {
					result._data[i][j] += _data[i][k] * other._data[k][j];
				}
			}
		}
		return result;
	}

	Matrix operator*(double scalar) {
		Matrix result(_cols, _rows);
		for (int i = 0; i < _rows; ++i) {
			for (int j = 0; j < _cols; ++j) {
				result._data[i][j] = _data[i][j] * scalar;
			}
		}
		return result;
	}

	Matrix  operator/(double scalar) {
		if (scalar == 0) {
			throw std::invalid_argument("Division by zero.");
		}
		Matrix result(_cols, _rows);
		for (int i = 0; i < _rows; ++i) {
			for (int j = 0; j < _cols; ++j) {
				result._data[i][j] = _data[i][j] / scalar;
			}
		}
		return result;
	}

	std::vector<double> operator[](int row) {
		return _data[row];
	}
};