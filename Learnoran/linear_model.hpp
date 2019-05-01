#ifndef _LINEAR_MODEL_HPP
#define _LINEAR_MODEL_HPP

//#define _SEQUENTIAL

#include <random>
#include <string>
#include <vector>
#include <unordered_map>
#include <iostream> // std::cout, only for debug
#include <omp.h>
#include <chrono>
#include <memory>

#include "predictor.hpp"
#include "polynomial.hpp"
#include "dataframe.hpp"
#include "encrypted_number.hpp"
#include "encryption_manager.hpp"

namespace Learnoran {
	class LinearModel : public Predictor {
	public:
		LinearModel(std::shared_ptr<EncryptionManager> encryption_manager = nullptr) : encryption_manager(encryption_manager) { }

		void encrypt_model(std::shared_ptr<EncryptionManager> encryption_manager) {
			// encrypts the plaintext model to obtain an encrypted model
			this->encryption_manager = encryption_manager;

			std::unordered_map<std::string, PolynomialTerm<double>> plain_terms = plaintext_model.get_terms();
			std::pair<std::string, PolynomialTerm<double>> plain_const_term = plaintext_model.get_constant_term();

			for (std::unordered_map<std::string, PolynomialTerm<double>>::const_iterator term = plain_terms.cbegin(); term != plain_terms.cend(); term++) {
				encrypted_model.add_term(encryption_manager->encrypt(term->second.coefficient), term->first, term->second.exponent);
			}
			encrypted_model.set_constant_term(encryption_manager->encrypt(plain_const_term.second.coefficient), plain_const_term.first);

			encrypted_zero = encryption_manager->encrypt(0.0);
		}

		// MARK: FIT (i.e. training)

		void fit(const Dataframe<double> & dataframe, const unsigned short epochs, const double learning_rate) override  {
			initialize_plaintext_model(dataframe);

			for (unsigned short epoch = 0; epoch < epochs; epoch++) {
				mse_batch_gd(dataframe, learning_rate);
				if (epoch % 10 == 0) {
					std::cout << "Epoch " << epoch << "/" << epochs << " - MSE for first 100 rows: " << compute_mean_square_error(dataframe, 100) << std::endl;
				}
			}
			std::cout << "Epoch " << epochs << "/" << epochs << " - MSE for first 100 rows: " << compute_mean_square_error(dataframe, 100) << std::endl;
		}

		void fit(const Dataframe<EncryptedNumber> & dataframe, const unsigned short epochs, const double learning_rate, const DecryptionManager * dec_man = nullptr) override {
			initialize_encrypted_model(dataframe);

			for (unsigned short epoch = 0; epoch < epochs; epoch++) {
				mse_batch_gd(dataframe, learning_rate, dec_man);
				std::cout << "epoch " << epoch + 1 << "/" << epochs << " completed" << std::endl;
			}
		}

		// MARK: PREDICTION

		double predict(const std::unordered_map<std::string, double> & features) override {
			return plaintext_model(features);
		}

		EncryptedNumber predict(const std::unordered_map<std::string, EncryptedNumber> & features, const DecryptionManager * dec_man = nullptr) override {
			return encrypted_model(features, encrypted_zero, dec_man);
		}

		double predict(const std::initializer_list<std::pair<std::string, double>> features)     {
			std::unordered_map<std::string, double> feature_map;

			for (const std::pair<std::string, double> & feature : features) {
				feature_map.insert(feature);
			}

			return predict(feature_map);
		}

		// MARK: MODEL ACCURACY ASSESSMENT

		double compute_mean_square_error(const Dataframe<double> & dataframe, const unsigned num_rows) override {
			DataframeShape shape = dataframe.shape();
			double loss = 0.0;

			for (unsigned row = 0; row < shape.rows; row++) {
				const double real_value = dataframe.get_row_label(row);
				std::unordered_map<std::string, double> row_features = dataframe.get_row_feature(row);

				const double model_error = plaintext_model(row_features) - real_value;
				loss += model_error * model_error;
			}
			loss *= 1.0 / (shape.rows);

			return loss;
		}

		EncryptedNumber compute_mean_square_error(const Dataframe<EncryptedNumber> & dataframe, const unsigned num_rows) override {
			DataframeShape shape = dataframe.shape();
			EncryptedNumber loss = encrypted_zero;

			for (unsigned row = 0; row < shape.rows; row++) {
				const EncryptedNumber real_value = dataframe.get_row_label(row);
				std::unordered_map<std::string, EncryptedNumber> row_features = dataframe.get_row_feature(row);

				EncryptedNumber model_error = encrypted_model(row_features, encrypted_zero) - real_value;
				loss += model_error * model_error;
			}
			loss *= 1.0 / (shape.rows);

			return loss;
		}
	private:
		// MARK: LINEAR_MODEL MEMBERS
		
		// Only one of the following models will be being used by an instance of this class as this class is intended
		// to be used with either double or EncryptedNumber types.
		Polynomial<double> plaintext_model;
		Polynomial<EncryptedNumber> encrypted_model;

		EncryptedNumber encrypted_zero;

		std::shared_ptr<EncryptionManager> encryption_manager;

		// MARK: LINEAR_MODEL PRIVATE MEMBER FUNCTION DECLERATIONS (and definitions)

		static double random_standard_normal() {
			std::mt19937 generator;
			std::normal_distribution<double> sn_distribution(0, 1);
			return sn_distribution(generator);
		}

		bool symbol_exists(const std::vector<std::string> variable_symbols, const std::string search_symbol) const {
			for (const std::string & symbol : variable_symbols) {
				if (symbol == search_symbol) {
					return true;
				}
			}
			return false;
		}

		void initialize_encrypted_model(const Dataframe<EncryptedNumber> & dataframe) {
			// construct a linear polynomial with random coefficients from the standard normal distribution
			const std::vector<std::string> variable_symbols = dataframe.get_feature_headers();

			for (const std::string & variable : variable_symbols) {
				encrypted_model.add_term(encryption_manager->encrypt(random_standard_normal()), variable, 1);
			}

			// add the bias term
			encrypted_model.set_constant_term(encryption_manager->encrypt(random_standard_normal()), "bias");

			// pre-compute the encrypted zero as it is used multiple times along the class methods
			this->encrypted_zero = encryption_manager->encrypt(0.0);
		}

		void initialize_plaintext_model(const Dataframe<double> & dataframe) {
			// construct a linear polynomial with random coefficients from the standard normal distribution
			const std::vector<std::string> variable_symbols = dataframe.get_feature_headers();

			for (const std::string & variable : variable_symbols) {
				plaintext_model.add_term(random_standard_normal(), variable, 1);
			}

			// add the bias term
			plaintext_model.set_constant_term(random_standard_normal(), "bias");
		}

		void print_model_coefficients(std::ostream & os) {
			// this function should only be used with plaintext models
			const std::unordered_map<std::string, PolynomialTerm<double>> terms = plaintext_model.get_terms();
			for (const std::pair<std::string, PolynomialTerm<double>> & term : terms) {
				os << term.first << '\t';
			}
			os << '\n';
			for (const std::pair<std::string, PolynomialTerm<double>> & term : terms) {
				os << term.second.coefficient << '\t';
			}
			os << '\n';
		}

		void mse_batch_gd(const Dataframe<double> & dataframe, const double learning_rate) {
			// applies gradient descent to MSE cost function

			DataframeShape shape = dataframe.shape();

			// go over each parameter and optimize them one by one
			for (std::pair<std::string, PolynomialTerm<double>> term : plaintext_model.get_terms()) {
				const std::string current_parameter = term.first;

				double derivative_cost_function = 0.0;

				// evaluate and reduce-sum the non-constant linear polynomial terms
#ifndef _SEQUENTIAL
#pragma omp parallel for
#endif
				for (int row = 0; row < shape.rows; row++) {
					const double & real_value = dataframe.get_row_label(row);
					const std::unordered_map<std::string, double> & row_features = dataframe.get_row_feature(row);

					const double model_prediction = plaintext_model(row_features);
					const double model_error = model_prediction - real_value;

					double inner_derivative = 1.0;
					const double normalizer = 1.0 / shape.rows;

					double current_row_error = model_error * inner_derivative;
					current_row_error *= normalizer;
#ifndef _SEQUENTIAL
#pragma omp critical
					{
#endif
						derivative_cost_function += current_row_error;
#ifndef _SEQUENTIAL
				}
#endif
			}

				// evaluate and add the constant term of the polynomial
				derivative_cost_function += plaintext_model.get_constant_term().second.coefficient;

				const double & current_parameter_value = plaintext_model[current_parameter];
				const double & parameter_new_value = current_parameter_value - (derivative_cost_function * learning_rate);

				plaintext_model[current_parameter] = parameter_new_value;
			}
		}

		void mse_batch_gd(const Dataframe<EncryptedNumber> & dataframe, const double learning_rate, const DecryptionManager * dec_man = nullptr) {
			// applies gradient descent to MSE cost function

			DataframeShape shape = dataframe.shape();

			// go over each parameter and optimize them one by one
			for (std::pair<std::string, PolynomialTerm<EncryptedNumber>> term : encrypted_model.get_terms()) {
				const std::string current_parameter = term.first;

				EncryptedNumber derivative_cost_function = encryption_manager->encrypt(0.0);
				
				// evaluate and reduce-sum the non-constant linear polynomial terms
#ifndef _SEQUENTIAL
#pragma omp parallel for
#endif
				for (int row = 0; row < shape.rows; row++) {
					const EncryptedNumber & real_value = dataframe.get_row_label(row);
					const std::unordered_map<std::string, EncryptedNumber> & row_features = dataframe.get_row_feature(row);

					const EncryptedNumber model_prediction = encrypted_model(row_features, encryption_manager->encrypt(0.0), dec_man);
					const EncryptedNumber model_error = model_prediction - real_value;

					EncryptedNumber inner_derivative = encryption_manager->encrypt(1.0);
					const double normalizer = 1.0 / shape.rows;

					EncryptedNumber current_row_error = model_error * inner_derivative;
					current_row_error *= normalizer;
#ifndef _SEQUENTIAL
#pragma omp critical
					{
#endif
						derivative_cost_function += current_row_error;
#ifndef _SEQUENTIAL
					}
#endif
				}

				// evaluate and add the constant term of the polynomial
				derivative_cost_function += encrypted_model.get_constant_term().second.coefficient;

				const EncryptedNumber & current_parameter_value = encrypted_model[current_parameter];
				const EncryptedNumber & parameter_new_value = current_parameter_value - (derivative_cost_function * learning_rate);

				encrypted_model[current_parameter] = parameter_new_value;
				std::cout << "Model parameter " << current_parameter << " updated" << std::endl;
			}
		}
	};
}

#endif