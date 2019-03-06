#include "stdafx.h"
#include "CppUnitTest.h"

#include "../Learnoran/polynomial.hpp"
#include "../Learnoran/encryption_manager.hpp"
#include "../Learnoran/encrypted_number.hpp"
#include "../Learnoran/decryption_manager.hpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Learnoran;

const double TOLERANCE = 0.00001;

namespace EncryptionTest
{
	TEST_CLASS(EncryptedArithmeticTest)
	{
	public:

		TEST_METHOD(SingleDoubleEncryption)
		{
			// TODO: Your test code here
			EncryptionManager enc_manager;
			DecryptionManager dec_manager(enc_manager.get_secret_key());

			const double plaintext = 2.37;

			EncryptedNumber ciphertext = enc_manager.encrypt(plaintext);
			
			double decrypted = dec_manager.decrypt(ciphertext);

			Assert::AreEqual(plaintext, decrypted, TOLERANCE, L"E(D(x)) must evaluate to x", LINE_INFO());
		}

		TEST_METHOD(Addition)
		{
			// TODO: Your test code here
			EncryptionManager enc_manager;
			DecryptionManager dec_manager(enc_manager.get_secret_key());

			const double value1 = 2.37;
			const double value2 = 5.8465;

			EncryptedNumber ciphertext1 = enc_manager.encrypt(value1);
			EncryptedNumber ciphertext2 = enc_manager.encrypt(value2);

			EncryptedNumber addition_result = ciphertext1 + ciphertext2;

			double decrypted_result = dec_manager.decrypt(addition_result);

			Assert::AreEqual(value1 + value2, decrypted_result, TOLERANCE, L"D(E(2.37)+E(5.8465)) must evaluate to correct result", LINE_INFO());
		}

		TEST_METHOD(Subtraction)
		{
			// TODO: Your test code here
			EncryptionManager enc_manager;
			DecryptionManager dec_manager(enc_manager.get_secret_key());

			const double value1 = 2.37;
			const double value2 = 5.8465;

			EncryptedNumber ciphertext1 = enc_manager.encrypt(value1);
			EncryptedNumber ciphertext2 = enc_manager.encrypt(value2);

			EncryptedNumber addition_result = ciphertext1 - ciphertext2;

			double decrypted_result = dec_manager.decrypt(addition_result);

			Assert::AreEqual(value1 - value2, decrypted_result, TOLERANCE, L"D(E(2.37)-E(5.8465)) must evaluate to correct result", LINE_INFO());
		}

		TEST_METHOD(Multiplication)
		{
			// TODO: Your test code here
			EncryptionManager enc_manager;
			DecryptionManager dec_manager(enc_manager.get_secret_key());

			const double value1 = 2.37;
			const double value2 = 5.8465;

			EncryptedNumber ciphertext1 = enc_manager.encrypt(value1);
			EncryptedNumber ciphertext2 = enc_manager.encrypt(value2);

			EncryptedNumber addition_result = ciphertext1 * ciphertext2;

			double decrypted_result = dec_manager.decrypt(addition_result);

			Assert::AreEqual(value1 * value2, decrypted_result, TOLERANCE, L"D(E(2.37)*E(5.8465)) must evaluate to correct result", LINE_INFO());
		}

		TEST_METHOD(MultipleMultiplication)
		{
			EncryptionManager enc_manager;
			DecryptionManager dec_manager(enc_manager.get_secret_key());

			const double value = 2;

			EncryptedNumber ciphertext = enc_manager.encrypt(value);
			EncryptedNumber accumulation_variable = enc_manager.encrypt(1.0);

			accumulation_variable = accumulation_variable * ciphertext;
			accumulation_variable = accumulation_variable * ciphertext;
			const double decrypted = dec_manager.decrypt(accumulation_variable);

			Assert::AreEqual(std::pow(value, 2), decrypted, L"Multiple multiplication failed", LINE_INFO());
		}

		TEST_METHOD(InplaceMultiplication)
		{
			EncryptionManager enc_manager;
			DecryptionManager dec_manager(enc_manager.get_secret_key());

			const double value1 = 4;
			const double value2 = 3;

			EncryptedNumber ciphertext1 = enc_manager.encrypt(value1);
			EncryptedNumber ciphertext2 = enc_manager.encrypt(value2);

			ciphertext1 *= ciphertext2;

			const double decrypted = dec_manager.decrypt(ciphertext1);
			
			Assert::AreEqual(value1 * value2, decrypted, TOLERANCE, L"In-place multiplication is yielding wrong results", LINE_INFO());
		}

		TEST_METHOD(InplaceMultipleMultiplication)
		{
			EncryptionManager enc_manager;
			DecryptionManager dec_manager(enc_manager.get_secret_key());

			const double value1 = 4;
			const double value2 = 3;

			EncryptedNumber ciphertext1 = enc_manager.encrypt(value1);
			EncryptedNumber ciphertext2 = enc_manager.encrypt(value2);

			// ciphertext1 = (ciphertext1 * ciphertext2^2)^2
			ciphertext1 *= ciphertext2;
			ciphertext1 *= ciphertext2;
			ciphertext1 *= ciphertext1;

			const double decrypted = dec_manager.decrypt(ciphertext1);

			Assert::AreEqual(std::pow(value1 * std::pow(value2, 2), 2), decrypted, TOLERANCE, L"In-place multiplication is yielding wrong results", LINE_INFO());
		}

		TEST_METHOD(InplacePlainMultiplication)
		{
			EncryptionManager enc_manager;
			DecryptionManager dec_manager(enc_manager.get_secret_key());

			const double value1 = 4;
			const double value2 = 3;

			EncryptedNumber ciphertext = enc_manager.encrypt(value1);

			ciphertext *= value2;

			const double decrypted = dec_manager.decrypt(ciphertext);

			Assert::AreEqual(value1 * value2, decrypted, TOLERANCE, L"In-place multiplication is yielding wrong results", LINE_INFO());
		}

		TEST_METHOD(InplaceAddition)
		{
			EncryptionManager enc_manager;
			DecryptionManager dec_manager(enc_manager.get_secret_key());

			const double value1 = 4.87;
			const double value2 = 3.86;

			EncryptedNumber ciphertext1 = enc_manager.encrypt(value1);
			EncryptedNumber ciphertext2 = enc_manager.encrypt(value2);

			ciphertext1 += ciphertext2;

			const double decrypted = dec_manager.decrypt(ciphertext1);

			Assert::AreEqual(value1 + value2, decrypted, TOLERANCE, L"In-place addition is yielding wrong results", LINE_INFO());
		}

		TEST_METHOD(InplaceMultipleAddition)
		{
			EncryptionManager enc_manager;
			DecryptionManager dec_manager(enc_manager.get_secret_key());

			const double value1 = 4.87;
			const double value2 = 3.86;

			EncryptedNumber ciphertext1 = enc_manager.encrypt(value1);
			EncryptedNumber ciphertext2 = enc_manager.encrypt(value2);

			// ciphertext1 = 2*(ciphertext1 + 2*ciphertext2)
			ciphertext1 += ciphertext2;
			ciphertext1 += ciphertext2;
			ciphertext1 += ciphertext1;

			const double decrypted = dec_manager.decrypt(ciphertext1);

			Assert::AreEqual(2*(value1 + 2* value2), decrypted, TOLERANCE, L"In-place addition is yielding wrong results", LINE_INFO());
		}

		TEST_METHOD(InplacePlainAddition)
		{
			EncryptionManager enc_manager;
			DecryptionManager dec_manager(enc_manager.get_secret_key());

			const double value1 = 7.846;
			const double value2 = 2.94;

			EncryptedNumber ciphertext1 = enc_manager.encrypt(value1);
			
			ciphertext1 += value2;

			const double decrypted = dec_manager.decrypt(ciphertext1);

			Assert::AreEqual(value1 + value2, decrypted, TOLERANCE, L"In-place plain addition is yielding wrong results", LINE_INFO());
		}

		TEST_METHOD(Exponentiation)
		{
			// TODO: Your test code here
			EncryptionManager enc_manager;
			DecryptionManager dec_manager(enc_manager.get_secret_key());

			const double base = 2.37;
			const unsigned exponent = 2;

			EncryptedNumber ciphertext = enc_manager.encrypt(base);
			EncryptedNumber raised_ciphertext = Learnoran::pow(ciphertext, exponent);

			double decrypted_result = dec_manager.decrypt(raised_ciphertext);

			Assert::AreEqual(std::pow(base, exponent), decrypted_result, TOLERANCE, L"D(E(2.37)^2) must evaluate to correct result", LINE_INFO());
		}

		TEST_METHOD(LargeExponentiation)
		{
			// TODO: Your test code here
			EncryptionManager enc_manager;
			DecryptionManager dec_manager(enc_manager.get_secret_key());

			const double base = 2;
			const unsigned exponent = 4;

			EncryptedNumber ciphertext = enc_manager.encrypt(base);
			EncryptedNumber raised_ciphertext = Learnoran::pow(ciphertext, exponent);

			double decrypted_result = dec_manager.decrypt(raised_ciphertext);

			Assert::AreEqual(std::pow(base, exponent), decrypted_result, TOLERANCE, L"D(E(2.37)^7) must evaluate to correct result", LINE_INFO());
		}

		TEST_METHOD(_16bitNumberEncryptionDecryption)
		{
			EncryptionManager enc_manager;
			DecryptionManager dec_manager(enc_manager.get_secret_key());

			const int plaintext = (1 << 15) + 456498;

			const EncryptedNumber ciphertext = enc_manager.encrypt(plaintext);
			const double decrypted = dec_manager.decrypt(ciphertext);

			Assert::AreEqual(plaintext, static_cast<int>(decrypted), L"16 bit integer decryption result mismatches plaintext", LINE_INFO());
		}

		TEST_METHOD(SEALmultiplication_power)
		{
			EncryptionManager enc_manager;
			DecryptionManager dec_manager(enc_manager.get_secret_key());

			const int base = 11;
			const unsigned exponent = 4;

			const EncryptedNumber ciphertext = enc_manager.encrypt(base);
			const EncryptedNumber raised_ciphertext = Learnoran::pow(ciphertext, exponent);
			const unsigned decrypted = static_cast<unsigned>(dec_manager.decrypt(raised_ciphertext));

			Assert::AreEqual(static_cast<unsigned>(std::pow(base, exponent)), decrypted, L"Exponentiation test failed", LINE_INFO());
		}

		TEST_METHOD(EncryptedPolynomialOperations)
		{
			EncryptionManager enc_manager;
			DecryptionManager dec_manager(enc_manager.get_secret_key());

			// Some evaluations on the encrypted polynomial f(x, y) = 2x^3 + y^2 + 1 will be performed

			Polynomial<EncryptedNumber> polynomial;
			polynomial.add_term(enc_manager.encrypt(2), "x", 3);
			polynomial.add_term(enc_manager.encrypt(1), "y", 2);
			polynomial.set_constant_term(enc_manager.encrypt(1), "__constant__");

			// polynomial will be evaluated for x = 3 and y = 2
			std::unordered_map<std::string, EncryptedNumber> eval_params({ { "x", enc_manager.encrypt(3) }, { "y", enc_manager.encrypt(2) } });
			const EncryptedNumber eval_result = polynomial(eval_params, enc_manager.encrypt(0), &dec_manager);

			const int eval_result_decrypted = dec_manager.decrypt(eval_result);
			Assert::AreEqual(59, eval_result_decrypted, L"f(x, y) = 2x^3 + y^2 + 1 has yielded a false result homomorphically");
		}
	};

	// EncryptedNumberTest is actually an integration test suite rather than a unit test suite
	TEST_CLASS(EncryptedNumberTest)
	{
	public:
		TEST_METHOD(CopyConstructorTest) {
			EncryptionManager enc_manager;
			DecryptionManager dec_manager(enc_manager.get_secret_key());

			const double value = 3.768;

			EncryptedNumber ciphertext1 = enc_manager.encrypt(value);
			EncryptedNumber ciphertext_copy = ciphertext1;

			double copy_decrypted = dec_manager.decrypt(ciphertext_copy);

			Assert::AreEqual(value, copy_decrypted, TOLERANCE, L"EncryptedNumber copy constructor is not working as expected", LINE_INFO());
		}

		TEST_METHOD(MoveConstructorTest) {
			EncryptionManager enc_manager;
			DecryptionManager dec_manager(enc_manager.get_secret_key());

			const double value1 = 3.768;
			const double value2 = 8.23324;

			EncryptedNumber ciphertext1 = enc_manager.encrypt(value1);
			EncryptedNumber ciphertext2 = enc_manager.encrypt(value2);

			EncryptedNumber product_result = ciphertext1 * ciphertext2; // invoked move constructor

			double result_decrypted = dec_manager.decrypt(product_result);

			Assert::AreEqual(value1 * value2, result_decrypted, TOLERANCE, L"EncryptedNumber copy constructor is not working as expected", LINE_INFO());
		}
	};

	TEST_CLASS(PolynomialTest)
	{
	public:

		TEST_METHOD(EvaluationOperatorSingleVariable)
		{
			// TODO: Your test code here
			Polynomial<double> polynomial;
			polynomial.add_term(3, "x", 2);

			Assert::AreEqual(75.0, polynomial({ {"x", 5} }), L"3x^2 must evaluate to 75 for x = 5", LINE_INFO());
			Assert::AreEqual(0.0, polynomial({ {"x", 0} }), L"3x^2 must evaluate to 0 for x = 0", LINE_INFO());
		}

		TEST_METHOD(EvaluationOperatorMultivariate)
		{
			Polynomial<double> polynomial;
			polynomial.add_term(9, "x", 3);
			polynomial.add_term(2, "y", 1);

			Assert::AreEqual(554.0, polynomial({ {"x", 4}, {"y", -11} }), L"9x^3 + 2y must evaluate to 554 for x=4 and y=-11", LINE_INFO());
			Assert::AreEqual(9.0, polynomial({ {"x", 1}, {"y", 0} }), L"9x^3 + 2y must evaluate to 9 for x=1 and y=0", LINE_INFO());
			Assert::AreEqual(0.0, polynomial({ {"x", 0}, {"y", 0} }), L"9x^3 + 2y must evaluate to 9 for x=0 and y=0", LINE_INFO());
		}

		TEST_METHOD(PartialDerivativeSingleVariable)
		{
			Polynomial<double> polynomial;
			polynomial.add_term(2, "x", 7);

			Polynomial<double> derived = polynomial.partial_derivative("x");

			Assert::AreEqual(896.0, derived({ {"x", 2} }), L"2^7 partial derivative WRT x must evaluate to 896 for x = 2", LINE_INFO());
		}

		TEST_METHOD(PartialDerivativeMultivariate)
		{
			Polynomial<double> polynomial;
			polynomial.add_term(143, "x", 9);
			polynomial.add_term(2, "y", 12);

			Polynomial<double> derived = polynomial.partial_derivative("y");

			Assert::AreEqual(4251528.0, derived({ {"y", 3} }), L"143x^9 + 2y^12 partial derivative WRT y must evaluate to 4251528 for y = 3", LINE_INFO());
		}
	};
}