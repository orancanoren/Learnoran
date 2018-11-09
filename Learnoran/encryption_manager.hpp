#ifndef _ENCRYPTION_MANAGER_HPP
#define _ENCRYPTION_MANAGER_HPP

#include <seal/seal.h>
#include <memory>
#include <vector>

#include "encrypted_number.hpp"
#include "dataframe.hpp"

namespace Learnoran {
	template <typename T>
	class EncryptionManager {
	public:
		EncryptionManager(size_t polynomial_modulus_degree = 2048, uint64_t plain_modulus = 1 << 8, const char * public_key_file = "", const char * secret_key_file = "") {//, const char * public_key_file = "", const char * secret_key_file = "") {
			/*
			If no public key file is given, EncryptionManager generates public and secret keys using SEAL
			Otherwise if only public key is given, secret key is not generated.
			*/

			// set encryption parameters
			seal::EncryptionParameters encryption_parameters(seal::scheme_type::BFV);
			encryption_parameters.set_poly_modulus_degree(polynomial_modulus_degree);
			encryption_parameters.set_coeff_modulus(seal::coeff_modulus_128(polynomial_modulus_degree));
			encryption_parameters.set_plain_modulus(plain_modulus);

			context = seal::SEALContext::Create(encryption_parameters);

			encoder = std::make_shared<seal::FractionalEncoder>(encryption_parameters.plain_modulus(), polynomial_modulus_degree, 1024, 1024);

			seal::KeyGenerator keygen(context);
			/* ======== COMMENTED OUT BECAUSE OF ISSUES WITH io_seal ==========
			IOSeal io_helper(public_key_file, secret_key_file);
			if (std::strcmp(public_key_file, "") == 0) {
				// generate both keys
				public_key = keygen.public_key();
				secret_key = keygen.secret_key();
				decryptor = new seal::Decryptor(context, secret_key);
			}
			else if (std::strcmp(secret_key_file, "") == 0) {
				// load the public key
				public_key = io_helper.read_public_key();
			}
			else {
				// load both keys
				public_key = io_helper.read_public_key();
				secret_key = io_helper.read_secret_key();
				decryptor = new seal::Decryptor(context, secret_key);
			}
			*/

			public_key = keygen.public_key();
			secret_key = keygen.secret_key();

			encryptor = new seal::Encryptor(context, public_key);
			decryptor = new seal::Decryptor(context, secret_key);
			evaluator = std::make_shared<seal::Evaluator>(context);
		}

		~EncryptionManager() {
			if (encryptor != nullptr) {
				delete encryptor;
			}
		}

		EncryptedNumber encrypt(const T & x) {
			seal::Plaintext plaintext = encoder->encode(x);
			seal::Ciphertext ciphertext;

			encryptor->encrypt(plaintext, ciphertext);

			return EncryptedNumber(ciphertext, evaluator, encoder);
		}

		T decrypt(const EncryptedNumber & ciphertext) {
			seal::Plaintext plaintext;
			decryptor->decrypt(ciphertext.ciphertext, plaintext);

			double result = encoder->decode(plaintext);

			return result;
		}

		Dataframe<EncryptedNumber> encrypt_dataframe(Dataframe<T> & df) {
			const DataframeShape shape = df.shape();

			std::vector<std::vector<EncryptedNumber>> encrypted_features(shape.rows);
			std::vector<EncryptedNumber> encrypted_labels(shape.rows);

#ifndef _SEQUENTIAL
#pragma omp parallel for
#endif
			for (int row = 0; row < shape.rows; row++) {
				const std::vector<T> & feature_row = df.get_row_feature_array(row);
				const unsigned feature_columns = shape.columns - 1;
				encrypted_features[row].resize(feature_columns);

				for (unsigned col = 0; col < feature_columns ; col++) {
					EncryptedNumber encrypted_feature_value = encrypt(feature_row[col]);
					encrypted_features[row][col] = encrypted_feature_value;
				}

				encrypted_labels[row] = encrypt(df.get_row_label(row));
			}

			Dataframe<EncryptedNumber> encrypted_df(encrypted_features, encrypted_labels, df.get_headers());
			return encrypted_df;
		}
	
		Dataframe<T> decrypt_dataframe(Dataframe<EncryptedNumber> & df) {
			const DataframeShape shape = df.shape();

			std::vector<std::vector<T>> decrypted_features(shape.rows);
			std::vector<T> decrypted_labels(shape.rows);

#ifndef _SEQUENTIAL
#pragma omp parallel for
#endif
			for (int row = 0; row < shape.rows; row++) {
				const std::vector<EncryptedNumber> & feature_row = df.get_row_feature_array(row);
				const unsigned feature_columns = shape.columns - 1;
				decrypted_features[row].resize(feature_columns);

				for (unsigned col = 0; col < feature_columns; col++) {
					T decrypted_feature_value = decrypt(feature_row[col]);
					decrypted_features[row][col] = decrypted_feature_value;
				}

				decrypted_labels[row] = decrypt(df.get_row_label(row));
			}

			Dataframe<T> decrypted_df(decrypted_features, decrypted_labels, df.get_headers());
			return decrypted_df;
		}
private:
		seal::Encryptor * encryptor;
		seal::Decryptor * decryptor;

		seal::PublicKey public_key;
		seal::SecretKey secret_key;

		std::shared_ptr<seal::SEALContext> context;
		std::shared_ptr<seal::Evaluator> evaluator;
		std::shared_ptr<seal::FractionalEncoder> encoder;
	};
}


#endif