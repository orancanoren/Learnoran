#include "stdafx.h"
#include "CppUnitTest.h"
#include "../Learnoran/polynomial.hpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace PolynomialTest
{		
	TEST_CLASS(PolynomialTest)
	{
	public:
		
		TEST_METHOD(EvaluationOperatorSingleVariable)
		{
			// TODO: Your test code here
			Polynomial polynomial;
			polynomial.add_term(3, "x", 2);

			Assert::AreEqual(75.0, polynomial({ {"x", 5} }), L"3x^2 must evaluate to 75 for x = 5", LINE_INFO());
			Assert::AreEqual(0.0, polynomial({ {"x", 0} }), L"3x^2 must evaluate to 0 for x = 0", LINE_INFO());
		}

		TEST_METHOD(EvaluationOperatorMultivariate)
		{
			Polynomial polynomial;
			polynomial.add_term(9, "x", 3);
			polynomial.add_term(2, "y", 1);

			Assert::AreEqual(554.0, polynomial({ {"x", 4}, {"y", -11} }), L"9x^3 + 2y must evaluate to 554 for x=4 and y=-11", LINE_INFO());
			Assert::AreEqual(9.0, polynomial({ {"x", 1}, {"y", 0} }), L"9x^3 + 2y must evaluate to 9 for x=1 and y=0", LINE_INFO());
			Assert::AreEqual(0.0, polynomial({ {"x", 0}, {"y", 0} }), L"9x^3 + 2y must evaluate to 9 for x=0 and y=0", LINE_INFO());
		}

		TEST_METHOD(PartialDerivativeSingleVariable)
		{
			Polynomial polynomial;
			polynomial.add_term(2, "x", 7);

			Polynomial derived = polynomial.partial_derivative("x");

			Assert::AreEqual(896.0, derived({ {"x", 2} }), L"2^7 partial derivative WRT x must evaluate to 896 for x = 2", LINE_INFO());
		}

		TEST_METHOD(PartialDerivativeMultivariate)
		{
			Polynomial polynomial;
			polynomial.add_term(143, "x", 9);
			polynomial.add_term(2, "y", 12);

			Polynomial derived = polynomial.partial_derivative("y");

			Assert::AreEqual(4251528.0, derived({ {"y", 3} }), L"143x^9 + 2y^12 partial derivative WRT y must evaluate to 4251528 for y = 3", LINE_INFO());
		}
	};
}