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
	};
}