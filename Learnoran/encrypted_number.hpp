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
			this->ciphertext = rhs.ciphertext;
			this->encoder = rhs.encoder;
			this->evaluator = rhs.evaluator;
		}

		// MARK: Operators

		EncryptedNumber & operator=(const EncryptedNumber & rhs) {
			this->ciphertext = rhs.ciphertext;
			this->evaluator = rhs.evaluator;
			return *this;
		}

		EncryptedNumber & operator+(const EncryptedNumber & rhs) const  {
			seal::Ciphertext result;

			evaluator->add(ciphertext, rhs.ciphertext, result);
			EncryptedNumber result_number(result, evaluator, encoder);
			return result_number;
		}

		EncryptedNumber & operator-(const EncryptedNumber & rhs) const  {
			seal::Ciphertext result;

			evaluator->negate(rhs.ciphertext, result);
			evaluator->add_inplace(result, ciphertext);
			
			EncryptedNumber result_number(result, evaluator, encoder);
			return result_number;
		}

		EncryptedNumber & operator*(const EncryptedNumber & rhs) const  {
			seal::Ciphertext result;

			evaluator->multiply(ciphertext, rhs.ciphertext, result);

			EncryptedNumber result_number(result, evaluator, encoder);
			return result_number;
		}

		EncryptedNumber & operator*(const double & rhs) const  {
			seal::Plaintext encoded_rhs = encoder->encode(rhs);
			seal::Ciphertext result;

			evaluator->multiply_plain(ciphertext, encoded_rhs, result);

			EncryptedNumber result_number(result, evaluator, encoder);
			return result_number;
		}

		EncryptedNumber & operator+=(const EncryptedNumber & rhs) {
			evaluator->multiply_inplace(ciphertext, rhs.ciphertext);

			return *this;
		}

		EncryptedNumber & operator+=(const double & rhs) {
			evaluator->add_plain_inplace(ciphertext, encoder->encode(rhs));
		}

		EncryptedNumber & operator*=(const EncryptedNumber & rhs) {
			evaluator->multiply_inplace(ciphertext, rhs.ciphertext);

			return *this;
		}

		EncryptedNumber & operator*=(const double & rhs) {
			evaluator->multiply_plain_inplace(ciphertext, encoder->encode(rhs));

			return *this;
		}

		// MARK: Members

		static EncryptedNumber & pow(const EncryptedNumber & base, unsigned exponent) {
			// naive implementation of raising base to exponent
			EncryptedNumber result = base;

			for (unsigned i = 1; i < exponent; i++) {
				result *= result;
			}

			return result;
		}

		seal::Ciphertext ciphertext;
	private:
		std::shared_ptr<seal::Evaluator> evaluator;
		std::shared_ptr<seal::FractionalEncoder> encoder;
	};
}

#endif