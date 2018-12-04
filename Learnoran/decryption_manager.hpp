#ifndef _DECRYPTION_MANAGER
#define _DECRYPTION_MANAGER

#include <seal/seal.h>
#include <memory>

#include "seal_parameters.hpp"
#include "encrypted_number.hpp"
#include "io_helper.hpp"
#include "dataframe.hpp"

namespace Learnoran {
	class DecryptionManager {
	public:
		DecryptionManager(const char * secret_key_file, const BFVParameters & parameters = BFVParameters(), const FractionalEncoderParameters & encoder_params = FractionalEncoderParameters()) 
			: integer_coeff_count(encoder_params.integer_coeff_count), fractional_coeff_count(encoder_params.fraction_coeff_count) {
			initialize_manager(parameters);
			
			IOhelper io_helper;
			io_helper.open_file(secret_key_file);
			seal::SecretKey secret_key = io_helper.read_secret_key(secret_key_file);

			decryptor = new seal::Decryptor(context, secret_key);
		}

		DecryptionManager(seal::SecretKey secret_key, const BFVParameters & parameters = BFVParameters(), const FractionalEncoderParameters & encoder_params = FractionalEncoderParameters()) 
			: integer_coeff_count(encoder_params.integer_coeff_count), fractional_coeff_count(encoder_params.fraction_coeff_count) {
			initialize_manager(parameters);

			decryptor = new seal::Decryptor(context, secret_key);
		}
	
		double decrypt(const EncryptedNumber & ciphertext) const {
			seal::Plaintext plaintext;
			decryptor->decrypt(ciphertext.ciphertext, plaintext);

			return encoder->decode(plaintext);
		}
	
		Dataframe<double> decrypt_dataframe(Dataframe<EncryptedNumber> & df) const {
			const DataframeShape shape = df.shape();

			std::vector<std::vector<double>> decrypted_features(shape.rows);
			std::vector<double> decrypted_labels(shape.rows);

#ifndef _SEQUENTIAL
#pragma omp parallel for
#endif
			for (int row = 0; row < shape.rows; row++) {
				const std::vector<EncryptedNumber> & feature_row = df.get_row_feature_array(row);
				const size_t feature_columns = shape.columns - 1;
				decrypted_features[row].resize(feature_columns);

				for (unsigned col = 0; col < feature_columns; col++) {
					double decrypted_feature_value = decrypt(feature_row[col]);
					decrypted_features[row][col] = decrypted_feature_value;
				}

				decrypted_labels[row] = decrypt(df.get_row_label(row));
			}

			Dataframe<double> decrypted_df(decrypted_features, decrypted_labels, df.get_headers());
			return decrypted_df;
		}
	
		int get_noise_budget_bits(const EncryptedNumber & ciphertext) const {
			return decryptor->invariant_noise_budget(ciphertext.ciphertext);
		}
	private:
		void initialize_manager(const BFVParameters & parameters) {
			seal::EncryptionParameters encryption_parameters(seal::scheme_type::BFV);
			encryption_parameters.set_poly_modulus_degree(parameters.polynomial_modulus_degree);
			encryption_parameters.set_coeff_modulus(seal::coeff_modulus_128(parameters.polynomial_modulus_degree));
			encryption_parameters.set_plain_modulus(parameters.plain_modulus);

			context = seal::SEALContext::Create(encryption_parameters);
			encoder = std::make_shared<seal::FractionalEncoder>(encryption_parameters.plain_modulus(), parameters.polynomial_modulus_degree, integer_coeff_count, fractional_coeff_count);
		}

		std::shared_ptr<seal::SEALContext> context;
		std::shared_ptr<seal::FractionalEncoder> encoder;

		const std::size_t & integer_coeff_count;
		const std::size_t & fractional_coeff_count;

		seal::Decryptor * decryptor;
	};
}

#endif