#include "stdafx.h"
#include "CppUnitTest.h"
#include "../NeuralNetwork/matrix.hpp"

#include <vector>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

double sample_map_func(const double x) {
	return x * x;
}

namespace NeuralNetworkTest
{		
	TEST_CLASS(MatrixTest)
	{
	public:
		
		TEST_METHOD(AdditionTest)
		{
			// TODO: Your test code here
			const unsigned rows = 12;
			const unsigned cols = 15;

			std::vector<std::vector<double>> matrix1(rows), matrix2(rows), result(rows);

			for (unsigned row = 0; row < rows; row++) {
				matrix1[row].resize(cols);
				matrix2[row].resize(cols);
				result[row].resize(cols);
				for (unsigned col = 0; col < cols; col++) {
					matrix1[row][col] = 2.23; // fill all elements of the first matrix with 2.23
					matrix2[row][col] = 3.35; // fill all elements of the second matrix with 3.35
					result[row][col] = 5.58;
				}
			}

			Matrix<double> mat1(matrix1);
			Matrix<double> mat2(matrix2);
			Matrix<double> res(result);

			Matrix<double> mat3 = mat1 + mat2;

			Assert::IsTrue(mat3 == res, L"ADDITION RESULT MISMATCHING", LINE_INFO());
		}

		TEST_METHOD(SubtractionTest)
		{
			// TODO: Your test code here
			const unsigned rows = 12;
			const unsigned cols = 15;

			std::vector<std::vector<double>> matrix1(rows), matrix2(rows), result(rows);

			for (unsigned row = 0; row < rows; row++) {
				matrix1[row].resize(cols);
				matrix2[row].resize(cols);
				result[row].resize(cols);
				for (unsigned col = 0; col < cols; col++) {
					matrix1[row][col] = 2.23; // fill all elements of the first matrix with 2.23
					matrix2[row][col] = 3.35; // fill all elements of the second matrix with 3.35
					result[row][col] = -1.12;
				}
			}

			Matrix<double> mat1(matrix1);
			Matrix<double> mat2(matrix2);
			Matrix<double> res(result);

			Matrix<double> mat3 = mat1 - mat2;

			Assert::IsTrue(mat3 == res, L"SUBTRACTION RESULT MISMATCHING", LINE_INFO());
		}

		TEST_METHOD(HadamardTest)
		{
			// TODO: Your test code here
			const unsigned rows = 12;
			const unsigned cols = 15;

			std::vector<std::vector<double>> matrix1(rows), matrix2(rows), result(rows);

			for (unsigned row = 0; row < rows; row++) {
				matrix1[row].resize(cols);
				matrix2[row].resize(cols);
				result[row].resize(cols);
				for (unsigned col = 0; col < cols; col++) {
					matrix1[row][col] = 2.23; // fill all elements of the first matrix with 2.23
					matrix2[row][col] = 3.35; // fill all elements of the second matrix with 3.35
					result[row][col] = 7.4705;
				}
			}

			Matrix<double> mat1(matrix1);
			Matrix<double> mat2(matrix2);
			Matrix<double> res(result);

			Matrix<double> mat3 = mat1 - mat2;

			for (unsigned row = 0; row < row; row++) {
				for (unsigned col = 0; col < cols; col++) {
					Assert::AreEqual(mat3[row][col], res[row][col], 0.0001, L"HADAMARD PRODUCT MISMATCHING", LINE_INFO());
				}
			}
		}

		TEST_METHOD(ScalarProductTest)
		{
			// TODO: Your test code here
			const unsigned rows = 12;
			const unsigned cols = 15;

			const unsigned scalar = 5.56;

			std::vector<std::vector<double>> matrix1(rows), result(rows);

			for (unsigned row = 0; row < rows; row++) {
				matrix1[row].resize(cols);;
				result[row].resize(cols);
				for (unsigned col = 0; col < cols; col++) {
					matrix1[row][col] = 2.23; // fill all elements of the first matrix with 2.23
					result[row][col] = scalar * 2.23;
				}
			}

			Matrix<double> mat1(matrix1);
			Matrix<double> res(result);

			Matrix<double> mat2 = mat1 * scalar;

			for (unsigned row = 0; row < row; row++) {
				for (unsigned col = 0; col < cols; col++) {
					Assert::AreEqual(mat2[row][col], scalar * 2.23, 0.0001, L"SCALAR PRODUCT MISMATCHING", LINE_INFO());
				}
			}
		}

		TEST_METHOD(TransposeTest)
		{
			// TODO: Your test code here
			const unsigned rows = 12;
			const unsigned cols = 15;

			std::vector<std::vector<double>> matrix(rows), result(rows);

			for (unsigned row = 0; row < rows; row++) {
				matrix[row].resize(cols);;
				result[row].resize(cols);
				for (unsigned col = 0; col < cols; col++) {
					matrix[row][col] = 2.23; // fill all elements of the first matrix with 2.23
				}
			}

			Matrix<double> mat(matrix);
			Matrix<double> res(result);

			Matrix<double> mat2 = mat.transpose();

			for (unsigned row = 0; row < row; row++) {
				for (unsigned col = 0; col < cols; col++) {
					Assert::AreEqual(mat2[col][row], mat[row][col], 0.0001, L"TRANSPOSED MATRIX MISMATCHING", LINE_INFO());
				}
			}
		}

		TEST_METHOD(MapTest)
		{
			// TODO: Your test code here
			const unsigned rows = 12;
			const unsigned cols = 15;

			std::vector<std::vector<double>> matrix(rows);

			for (unsigned row = 0; row < rows; row++) {
				matrix[row].resize(cols);;
				for (unsigned col = 0; col < cols; col++) {
					matrix[row][col] = 4.23; // fill all elements of the first matrix with 2.23
				}
			}

			const double mapped_value = sample_map_func(4.23);

			Matrix<double> mat(matrix);
			Matrix<double> res;

			res = mat.map(&sample_map_func);

			for (unsigned row = 0; row < row; row++) {
				for (unsigned col = 0; col < cols; col++) {
					Assert::AreEqual(mapped_value, res[row][col], 0.0001, L"MATRIX FUNCTION MAPPING RESULT MISMATCHING", LINE_INFO());
				}
			}
		}

		TEST_METHOD(DotProductTest)
		{
			// TODO: Your test code here
			const unsigned rows = 2;
			const unsigned cols = 3;

			std::vector<std::vector<double>> matrix(rows), matrix2(cols);

			for (unsigned i = 0; i < rows; i++) {
				matrix[i].resize(cols);
			}
			
			for (unsigned i = 0; i < cols; i++) {
				matrix2[i].resize(rows);
			}

			matrix[0][0] = 2.3;
			matrix[0][1] = 5;
			matrix[0][2] = 6;
			matrix[1][0] = 4;
			matrix[1][1] = -1.7;
			matrix[1][2] = 0;

			matrix2[0][0] = 1;
			matrix2[0][1] = 2;
			matrix2[1][0] = 3;
			matrix2[1][1] = 4;
			matrix2[2][0] = 5;
			matrix2[2][1] = 6;

			

			Matrix<double> mat1(matrix), mat2(matrix2);
			std::vector<std::vector<double>> res = mat1.dot(mat2).get_vector();

			Assert::AreEqual(static_cast<unsigned>(res.size()), 2U, L"DOT PRODUCT SHAPE INVALID", LINE_INFO());
			Assert::AreEqual(static_cast<unsigned>(res[0].size()), 2U, L"DOT PRODUCT SHAPE INVALID", LINE_INFO());

			Assert::AreEqual(47.3, res[0][0], 0.001, L"Entry [0][0] invalid", LINE_INFO());
			Assert::AreEqual(60.6, res[0][1], 0.001 ,L"Entry [0][1] invalid", LINE_INFO());
			Assert::AreEqual(-1.1, res[1][0], 0.001, L"Entry [1][0] invalid", LINE_INFO());
			Assert::AreEqual(1.2, res[1][1], 0.001, L"Entry [1][1] invalid", LINE_INFO());
		}

		TEST_METHOD(GetVectorTest)
		{
			// TODO: Your test code here
			const unsigned rows = 12;
			const unsigned cols = 15;

			std::vector<std::vector<double>> matrix(rows);

			for (unsigned row = 0; row < rows; row++) {
				matrix[row].resize(cols, 4.23);
			}

			Matrix<double> mat(matrix);

			std::vector<std::vector<double>> retrieved_vector = mat.get_vector();

			for (unsigned int row = 0; row < rows; row++) {
				Assert::IsTrue(retrieved_vector[row] == matrix[row]);
			}
		}
	
		TEST_METHOD(CopyConstructorTest)
		{
			const unsigned rows = 12;
			const unsigned cols = 15;

			std::vector<std::vector<double>> matrix(rows);

			for (unsigned row = 0; row < rows; row++) {
				matrix[row].resize(cols, 4.23);
			}

			Matrix<double> mat(matrix);
			Matrix<double> copy;
			copy = mat;

			for (unsigned row = 0; row < rows; row++) {
				for (unsigned col = 0; col < cols; col++) {
					Assert::AreEqual(mat[row][col], copy[row][col], 0.0001, L"COPIED MATRIX MISMATCHES THE ORIGINAL MATRIX", LINE_INFO());
				}
			}
		}

		TEST_METHOD(MoveConstructorTest)
		{
			const unsigned rows = 12;
			const unsigned cols = 15;

			std::vector<std::vector<double>> matrix(rows);

			for (unsigned row = 0; row < rows; row++) {
				matrix[row].resize(cols, 4.23);
			}

			Matrix<double> mat(matrix);
			Matrix<double> copy = mat;

			for (unsigned row = 0; row < rows; row++) {
				for (unsigned col = 0; col < cols; col++) {
					Assert::AreEqual(mat[row][col], copy[row][col], 0.0001, L"COPIED MATRIX MISMATCHES THE ORIGINAL MATRIX", LINE_INFO());
				}
			}
		}
	};
}