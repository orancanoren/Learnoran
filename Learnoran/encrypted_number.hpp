#ifndef _ENCRYPTED_NUMBER_HPP
#define _ENCRYPTED_NUMBER_HPP

#include <memory>
#include <seal/seal.h>

namespace Learnoran {
	class EncryptedNumber {
	public:
		// MARK: Constructors
		EncryptedNumber() { }

		EncryptedNumber(seal::Ciphertext ciphertext, std::shared_ptr<seal::Evaluator> evaluator, std::shared_ptr<seal::FractionalEncoder> encoder)
			: ciphertext(ciphertext), evaluator(evaluator), encoder(encoder) {
		}

		// Copy constructor
		EncryptedNumber(const EncryptedNumber & rhs) {
			*this = rhs;
		}

		// MARK: Operators

		EncryptedNumber & operator=(const EncryptedNumber & rhs) {
			this->ciphertext = rhs.ciphertext;
			this->evaluator = rhs.evaluator;
			return *this;
		}

		EncryptedNumber operator+(const EncryptedNumber & rhs) {
			seal::Ciphertext result;

			evaluator->add(ciphertext, rhs.ciphertext, result);
			EncryptedNumber result_number(result, evaluator, encoder);
			return result_number;
		}

		EncryptedNumber operator-(const EncryptedNumber & rhs) {
			seal::Ciphertext result;

			evaluator->negate(rhs.ciphertext, result);
			evaluator->add_inplace(result, ciphertext);
			
			EncryptedNumber result_number(result, evaluator, encoder);
			return result_number;
		}

		EncryptedNumber operator*(const EncryptedNumber & rhs) {
			seal::Ciphertext result;

			evaluator->multiply(ciphertext, rhs.ciphertext, result);

			EncryptedNumber result_number(result, evaluator, encoder);
			return result_number;
		}

		
		EncryptedNumber operator*(const double & rhs) {
			seal::Plaintext encoded_rhs = encoder->encode(rhs);
			seal::Ciphertext result;

			evaluator->multiply_plain(ciphertext, encoded_rhs, result);

			EncryptedNumber result_number(result, evaluator, encoder);
			return result_number;
		}

		// MARK: Members

		seal::Ciphertext ciphertext;
	private:
		std::shared_ptr<seal::Evaluator> evaluator;
		std::shared_ptr<seal::FractionalEncoder> encoder;
	};
}

#endif