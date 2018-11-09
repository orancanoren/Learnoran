#include "stdafx.h"
#include "CppUnitTest.h"
#include "Learnoran/encryption_manager.hpp"
#include "Learnoran/encrypted_number.hpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Learnoran;

namespace EncryptionTest
{
	TEST_CLASS(EncryptionTest)
	{
	public:

		TEST_METHOD(SingleDoubleEncryption)
		{
			// TODO: Your test code here
			EncryptionManager<double> manager;

			const double plaintext = 2.37;

			EncryptedNumber ciphertext = manager.encrypt(plaintext);
			
			double decrypted = manager.decrypt(ciphertext);

			Assert::AreEqual(plaintext, decrypted, 0.00001, L"E(D(x)) must evaluate to x", LINE_INFO());
		}

		TEST_METHOD(HomomorphicArithmeticAddition)
		{
			// TODO: Your test code here
			EncryptionManager<double> manager;

			const double value1 = 2.37;
			const double value2 = 5.8465;

			EncryptedNumber ciphertext1 = manager.encrypt(value1);
			EncryptedNumber ciphertext2 = manager.encrypt(value2);

			EncryptedNumber addition_result = ciphertext1 + ciphertext2;

			double decrypted_result = manager.decrypt(addition_result);

			Assert::AreEqual(value1 + value2, decrypted_result, 0.00001, L"D(E(2.37)+E(5.8465)) must evaluate to correct result", LINE_INFO());
		}

		TEST_METHOD(HomomorphicArithmeticSubtraction)
		{
			// TODO: Your test code here
			EncryptionManager<double> manager;

			const double value1 = 2.37;
			const double value2 = 5.8465;

			EncryptedNumber ciphertext1 = manager.encrypt(value1);
			EncryptedNumber ciphertext2 = manager.encrypt(value2);

			EncryptedNumber addition_result = ciphertext1 - ciphertext2;

			double decrypted_result = manager.decrypt(addition_result);

			Assert::AreEqual(value1 - value2, decrypted_result, 0.00001, L"D(E(2.37)-E(5.8465)) must evaluate to correct result", LINE_INFO());
		}

		TEST_METHOD(HomomorphicArithmeticMultiplication)
		{
			// TODO: Your test code here
			EncryptionManager<double> manager;

			const double value1 = 2.37;
			const double value2 = 5.8465;

			EncryptedNumber ciphertext1 = manager.encrypt(value1);
			EncryptedNumber ciphertext2 = manager.encrypt(value2);

			EncryptedNumber addition_result = ciphertext1 * ciphertext2;

			double decrypted_result = manager.decrypt(addition_result);

			Assert::AreEqual(value1 * value2, decrypted_result, 0.00001, L"D(E(2.37)*E(5.8465)) must evaluate to correct result", LINE_INFO());
		}
	};
}