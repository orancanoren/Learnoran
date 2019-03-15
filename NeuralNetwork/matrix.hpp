#ifndef _MATRIX_HPP
#define _MATRIX_HPP

#include <vector>
#include <iostream>
#include <assert.h>
#include <sstream>
#include <utility>

template <typename T>
class Matrix {
public:
	// CONSTRUCTORS
	Matrix();
	Matrix(const unsigned height, const unsigned width);
	Matrix(std::vector<std::vector<T>> const & matrix_array);
	Matrix(const Matrix && rhs); // move
	Matrix(const Matrix & rhs); // copy

	std::pair<unsigned, unsigned> get_shape() const;

	std::vector<std::vector<T>> get_vector() const;

	void print(std::ostream & flux) const; // pretty print the matrix

	Matrix map(T(*function)(T)) const; // apply a function to every element

	Matrix transpose() const;

	Matrix dot(Matrix const & rhs); // dot multiplication

	// GETTER AND SETTER OPERATORS
	Matrix & operator=(Matrix const & rhs);
	std::vector<T> & operator[](const unsigned row);

	// LOGIC OPERATORS
	bool operator==(Matrix const & rhs);

	// ARITHMETIC OPERATORS
	Matrix operator*(const double scalar); // scalar multiplication
	Matrix operator*(Matrix const & rhs); // hadamard product
	Matrix operator+(Matrix const & rhs); // addition
	Matrix operator-(Matrix const & rhs); // subtraction

private:
	std::vector<std::vector<T>> matrix_array;
	unsigned width;
	unsigned height;
};

template <typename T>
std::pair<unsigned, unsigned> Matrix<T>::get_shape() const {
	return std::make_pair(height, width);
}

template <typename T>
std::vector<std::vector<T>> Matrix<T>::get_vector() const {
	return matrix_array;
}

template <typename T>
std::ostream & operator<<(std::ostream & flux, Matrix<T> const & matrix);

template <typename T>
Matrix<T>::Matrix() { }

template <typename T>
Matrix<T>::Matrix(const unsigned height, const unsigned width) : height(height), width(width) {
	matrix_array.resize(height);
	for (unsigned row = 0; row < height; row++) {
		matrix_array[row].resize(width);
	}
}

template <typename T>
Matrix<T>::Matrix(std::vector<std::vector<T>> const & matrix_array) : height(matrix_array.size()), width(matrix_array[0].size()) {
	assert(matrix_array.size() != 0);
	this->matrix_array = matrix_array;
}

template <typename T>
Matrix<T>::Matrix(const Matrix && rhs) : height(rhs.height), width(rhs.width) {
	this->matrix_array = rhs.matrix_array;
}

template <typename T>
Matrix<T>::Matrix(const Matrix & rhs) : height(rhs.height), width(rhs.width) {
	this->matrix_array = rhs.matrix_array;
}

// Scalar multiplication
template <typename T>
Matrix<T> Matrix<T>::operator*(const double scalar) {
	Matrix result(height, width);

	for (unsigned row = 0; row < height; row++) {
		for (unsigned col = 0; col < width; col++) {
			result.matrix_array[row][col] = matrix_array[row][col] * scalar;
		}
	}

	return result;
}

// Addition
template <typename T>
Matrix<T> Matrix<T>::operator+(Matrix<T> const & rhs) {
	assert(height == rhs.height && width == rhs.width);

	Matrix result(height, width);

	for (unsigned row = 0; row < height; row++) {
		for (unsigned col = 0; col < width; col++) {
			result.matrix_array[row][col] = matrix_array[row][col] + rhs.matrix_array[row][col];
		}
	}

	return result;
}

// Subtraction
template <typename T>
Matrix<T> Matrix<T>::operator-(Matrix<T> const & rhs) {
	assert(height == rhs.height && width == rhs.width);

	Matrix result(height, width);

	for (unsigned row = 0; row < height; row++) {
		for (unsigned col = 0; col < width; col++) {
			result.matrix_array[row][col] = matrix_array[row][col] - rhs.matrix_array[row][col];
		}
	}

	return result;
}

// Hadamard product
template <typename T>
Matrix<T> Matrix<T>::operator*(Matrix<T> const & rhs) {
	assert(height == rhs.height && width == rhs.width);

	Matrix result(height, width);

	for (unsigned row = 0; row < height; row++) {
		for (unsigned col = 0; col < width; col++) {
			result.matrix_array[row][col] = matrix_array[row][col] * rhs.matrix_array[row][col];
		}
	}

	return result;
}

// Dot product
template <typename T>
Matrix<T> Matrix<T>::dot(Matrix<T> const & rhs) {
	assert(width == rhs.height);

	Matrix result(height, rhs.width);

	for (unsigned row = 0; row < height; row++) {
		for (unsigned rhs_col = 0; rhs_col < rhs.width; rhs_col++) {

			T result_element = 0.0;

			for (unsigned col = 0; col < width; col++) {
				result_element += matrix_array[row][col] * rhs.matrix_array[col][rhs_col];
			}

			result[row][rhs_col] = result_element;
		}
	}

	return result;
}

// Transpose
template <typename T>
Matrix<T> Matrix<T>::transpose() const {
	Matrix result(width, height);

	for (unsigned row = 0; row < height; row++) {
		for (unsigned col = 0; col < width; col++) {
			result[col][row] = matrix_array[row][col];
		}
	}

	return result;
}

template <typename T>
void Matrix<T>::print(std::ostream & flux) const {
	for (unsigned row = 0; row < height; row++) {
		for (unsigned col = 0; col < width; col++) {
			flux << matrix_array[row][col] << ' ';
		}
		flux << '\n';
	}
}

template <typename T>
Matrix<T> Matrix<T>::map(T(*function)(T)) const {
	Matrix result(height, width);

	for (unsigned row = 0; row < height; row++) {
		for (unsigned col = 0; col < width; col++) {
			result.matrix_array[row][col] = (*function)(matrix_array[row][col]);
		}
	}

	return result;
}

template <typename T>
std::ostream & operator<<(std::ostream & flux, Matrix<T> const & matrix) {
	matrix.print(flux);
	return flux;
}

template <typename T>
bool Matrix<T>::operator==(Matrix<T> const & rhs) {
	if (height != rhs.height || width != rhs.width) {
		return false;
	}

	for (unsigned row = 0; row < height; row++) {
		for (unsigned col = 0; col < width; col++) {
			if (matrix_array[row][col] != rhs.matrix_array[row][col]) {
				return false;
			}
		}
	}

	return true;
}

template <typename T>
Matrix<T> & Matrix<T>::operator=(const Matrix<T> & rhs) {
	matrix_array = rhs.matrix_array;
	height = rhs.height;
	width = rhs.width;

	return *this;
}

template <typename T>
std::vector<T> & Matrix<T>::operator[](const unsigned row) {
	return matrix_array[row];
}

#endif