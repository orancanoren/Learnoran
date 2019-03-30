#ifndef _MATRIX_HPP
#define _MATRIX_HPP

#include <vector>
#include <iostream>
#include <assert.h>
#include <sstream>
#include <utility>

struct Shape {
	Shape(unsigned rows, unsigned cols) : rows(rows), cols(cols) { }

	unsigned rows;
	unsigned cols;
};

template <typename T>
class Matrix {
public:
	// CONSTRUCTORS
	Matrix();
	Matrix(const unsigned rows, const unsigned cols);
	Matrix(const std::vector<std::vector<T>> & matrix_array);
	Matrix(const Matrix && rhs); // move
	Matrix(const Matrix & rhs); // copy

	Shape get_shape() const;

	std::vector<std::vector<T>> get_vector() const;

	std::vector<T> get_1d_vector() const;

	std::vector<T> get_col(const unsigned col) const;

	void print(std::ostream & flux) const; // pretty print the matrix

	Matrix map(T(*function)(T)) const; // apply a function to every element

	Matrix map(T(*function)(T, T), T param) const;

	Matrix transpose() const;

	Matrix dot(Matrix const & rhs); // dot multiplication

	// GETTER AND SETTER OPERATORS
	Matrix & operator=(Matrix const & rhs);
	std::vector<T> & operator[](const unsigned row);
	std::vector<T> operator[](const unsigned row) const;

	// LOGIC OPERATORS
	bool operator==(Matrix const & rhs);

	// ARITHMETIC OPERATORS
	Matrix operator*(const double scalar); // scalar multiplication
	Matrix operator*(Matrix const & rhs); // hadamard product
	Matrix operator+(Matrix const & rhs); // addition
	Matrix operator-(Matrix const & rhs); // subtraction

private:
	std::vector<std::vector<T>> matrix_array;
	unsigned cols;
	unsigned rows;
};

template <typename T>
std::vector<T> Matrix<T>::get_col(const unsigned col) const {
	std::vector<T> column(rows);
	
	for (unsigned i = 0; i < rows; i++) {
		column[i] = matrix_array[i][col];
	}

	return column;
}

template <typename T>
std::vector<T> Matrix<T>::operator[](const unsigned row) const{
	return matrix_array[row];
}

template <typename T>
std::vector<T> Matrix<T>::get_1d_vector() const {
	assert(cols == 1 || rows == 1);

	unsigned output_size = (cols == 1 ? rows : cols);

	std::vector<T> col_vector(output_size);

	if (cols == 1) {
		for (unsigned i = 0; i < rows; i++) {
			col_vector[i] = matrix_array[i][0];
		}
	}
	else {
		col_vector = matrix_array[0];
	}

	return col_vector;
}

template <typename T>
Shape Matrix<T>::get_shape() const {
	return Shape(rows, cols);
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
Matrix<T>::Matrix(const unsigned rows, const unsigned cols) : rows(rows), cols(cols) {
	matrix_array.resize(rows);
	for (unsigned row = 0; row < rows; row++) {
		matrix_array[row].resize(cols);
	}
}

template <typename T>
Matrix<T>::Matrix(const std::vector<std::vector<T>> & matrix_array) : rows(matrix_array.size()), cols(matrix_array[0].size()) {
	assert(matrix_array.size() != 0);
	this->matrix_array = matrix_array;
}

template <typename T>
Matrix<T>::Matrix(const Matrix && rhs) : rows(rhs.rows), cols(rhs.cols) {
	this->matrix_array = rhs.matrix_array;
}

template <typename T>
Matrix<T>::Matrix(const Matrix & rhs) : rows(rhs.rows), cols(rhs.cols) {
	this->matrix_array = rhs.matrix_array;
}

// Scalar multiplication
template <typename T>
Matrix<T> Matrix<T>::operator*(const double scalar) {
	Matrix result(rows, cols);

	for (unsigned row = 0; row < rows; row++) {
		for (unsigned col = 0; col < cols; col++) {
			result.matrix_array[row][col] = matrix_array[row][col] * scalar;
		}
	}

	return result;
}

// Addition
template <typename T>
Matrix<T> Matrix<T>::operator+(Matrix<T> const & rhs) {
	assert(rows == rhs.rows && cols == rhs.cols);

	Matrix result(rows, cols);

	for (unsigned row = 0; row < rows; row++) {
		for (unsigned col = 0; col < cols; col++) {
			result.matrix_array[row][col] = matrix_array[row][col] + rhs.matrix_array[row][col];
		}
	}

	return result;
}

// Subtraction
template <typename T>
Matrix<T> Matrix<T>::operator-(Matrix<T> const & rhs) {
	assert(rows == rhs.rows && cols == rhs.cols);

	Matrix result(rows, cols);

	for (unsigned row = 0; row < rows; row++) {
		for (unsigned col = 0; col < cols; col++) {
			result.matrix_array[row][col] = matrix_array[row][col] - rhs.matrix_array[row][col];
		}
	}

	return result;
}

// Hadamard product
template <typename T>
Matrix<T> Matrix<T>::operator*(Matrix<T> const & rhs) {
	assert(rows == rhs.rows && cols == rhs.cols);

	Matrix result(rows, cols);

	for (unsigned row = 0; row < rows; row++) {
		for (unsigned col = 0; col < cols; col++) {
			result.matrix_array[row][col] = matrix_array[row][col] * rhs.matrix_array[row][col];
		}
	}

	return result;
}

// Dot product
template <typename T>
Matrix<T> Matrix<T>::dot(Matrix<T> const & rhs) {
	assert(cols == rhs.rows);

	Matrix result(rows, rhs.cols);

	for (unsigned row = 0; row < rows; row++) {
		for (unsigned rhs_col = 0; rhs_col < rhs.cols; rhs_col++) {

			T result_element = 0.0;

			for (unsigned i = 0; i < cols; i++) {
				result_element += matrix_array[row][i] * rhs.matrix_array[i][rhs_col];
			}

			result[row][rhs_col] = result_element;
		}
	}

	return result;
}

// Transpose
template <typename T>
Matrix<T> Matrix<T>::transpose() const {
	Matrix result(cols, rows);

	for (unsigned row = 0; row < rows; row++) {
		for (unsigned col = 0; col < cols; col++) {
			result[col][row] = matrix_array[row][col];
		}
	}

	return result;
}

template <typename T>
void Matrix<T>::print(std::ostream & flux) const {
	for (unsigned row = 0; row < rows; row++) {
		for (unsigned col = 0; col < cols; col++) {
			flux << matrix_array[row][col] << ' ';
		}
		flux << '\n';
	}
}

template <typename T>
Matrix<T> Matrix<T>::map(T(*function)(T)) const {
	Matrix result(rows, cols);

	for (unsigned row = 0; row < rows; row++) {
		for (unsigned col = 0; col < cols; col++) {
			result.matrix_array[row][col] = (*function)(matrix_array[row][col]);
		}
	}

	return result;
}

template <typename T>
Matrix<T> Matrix<T>::map(T(*function)(T, T), T param) const {
	Matrix result(rows, cols);

	for (unsigned row = 0; row < rows; row++) {
		for (unsigned col = 0; col < cols; col++) {
			result.matrix_array[row][col] = (*function)(matrix_array[row][col], param);
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
	if (rows != rhs.rows || cols != rhs.cols) {
		return false;
	}

	for (unsigned row = 0; row < rows; row++) {
		for (unsigned col = 0; col < cols; col++) {
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
	rows = rhs.rows;
	cols = rhs.cols;

	return *this;
}

template <typename T>
std::vector<T> & Matrix<T>::operator[](const unsigned row) {
	return matrix_array[row];
}

#endif