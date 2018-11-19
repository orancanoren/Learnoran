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

#include "predictor.hpp"
#include "polynomial.hpp"
#include "dataframe.hpp"
#include "encrypted_number.hpp"
#include "encryption_manager.hpp"

namespace Learnoran {
	class LinearModel : public Predictor {
	public:
		LinearModel(const EncryptionManager & encryption_manager) : encryption_manager(encryption_manager) { }

		// MARK: FIT (i.e. training)

		void fit(const Dataframe<double> & dataframe, const unsigned short epochs, const double learning_rate) override  {
			initialize_plaintext_model(dataframe);

			for (unsigned short epoch = 0; epoch < epochs; epoch++) {
				mse_batch_gd(dataframe, learning_rate);
				if (epoch % 10 == 0) {
					std::cout << "loss after epoch " << epoch << ": " << compute_mean_square_error(dataframe) << std::endl;
				}
			}
			std::cout << "final loss: " << compute_mean_square_error(dataframe) << std::endl;
		}

		void fit(const Dataframe<EncryptedNumber> & dataframe, const unsigned short epochs, const double learning_rate) override {
			initialize_encrypted_model(dataframe);

			for (unsigned short epoch = 0; epoch < epochs; epoch++) {
				mse_batch_gd(dataframe, learning_rate);
				std::cout << "epoch " << epoch + 1 << "/" << epochs << " completed" << std::endl;
			}
		}

		// MARK: PREDICTION

		double predict(const std::unordered_map<std::string, double> features) const override  {
			return plaintext_model(features);
		}

		EncryptedNumber predict(const std::unordered_map<std::string, EncryptedNumber> features, EncryptedNumber encrypted_zero) const override {
			return encrypted_model(features, encrypted_zero);
		}

		double predict(const std::initializer_list<std::pair<std::string, double>> features) const    {
			std::unordered_map<std::string, double> feature_map;

			for (const std::pair<std::string, double> & feature : features) {
				feature_map.insert(feature);
			}

			return predict(feature_map);
		}

		// MARK: MODEL ACCURACY ASSESSMENT

		double compute_mean_square_error(const Dataframe<double> & dataframe) const;

		EncryptedNumber compute_mean_square_error(const Dataframe<EncryptedNumber> & dataframe, EncryptedNumber encrypted_zero) const;
	private:
		// MARK: LINEAR_MODEL MEMBERS
		
		// Only one of the following models will be being used by an instance of this class as this class is intended
		// to be used with either double or EncryptedNumber types.
		Polynomial<double> plaintext_model;
		Polynomial<EncryptedNumber> encrypted_model;

		const EncryptionManager & encryption_manager;

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
				encrypted_model.add_term(encryption_manager.encrypt(random_standard_normal()), variable, 1);
			}

			// add the bias term
			encrypted_model.set_constant_term(encryption_manager.encrypt(random_standard_normal()), "bias");
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
				const unsigned current_parameter_exponent = term.second.exponent;

				double derivative_cost_function = 0.0;
#ifndef _SEQUENTIAL
#pragma omp parallel for
#endif
				for (int row = 0; row < shape.rows; row++) {
					const double real_value = dataframe.get_row_label(row);
					std::unordered_map<std::string, double> row_features = dataframe.get_row_feature(row);

					const double model_prediction = plaintext_model(row_features);
					const double model_error = model_prediction - real_value;

					double inner_derivative = 1;
					if (current_parameter != "bias") {
						inner_derivative = std::pow(row_features[current_parameter], current_parameter_exponent);
					}

					const double normalizer = 1.0 / shape.rows;

					double current_row_error = normalizer * model_error * inner_derivative;
#ifndef _SEQUENTIAL
#pragma omp atomic
#endif
					derivative_cost_function += current_row_error;
				}

				double current_parameter_value = plaintext_model[current_parameter];
				double parameter_new_value = current_parameter_value - (learning_rate * derivative_cost_function);


				plaintext_model[current_parameter] = parameter_new_value;
			}
		}

		void mse_batch_gd(const Dataframe<EncryptedNumber> & dataframe, const double learning_rate) {
			// applies gradient descent to MSE cost function

			DataframeShape shape = dataframe.shape();

			// go over each parameter and optimize them one by one
			for (std::pair<std::string, PolynomialTerm<EncryptedNumber>> term : encrypted_model.get_terms()) {
				const std::string current_parameter = term.first;

				std::cout << "current parameter: " << current_parameter << std::endl;

				EncryptedNumber derivative_cost_function = encryption_manager.encrypt(0.0);
				
				// evaluate and reduce-sum the non-constant linear polynomial terms
#ifndef _SEQUENTIAL
#pragma omp parallel for
#endif
				for (int row = 0; row < shape.rows; row++) {
					const EncryptedNumber & real_value = dataframe.get_row_label(row);
					const std::unordered_map<std::string, EncryptedNumber> & row_features = dataframe.get_row_feature(row);

					const EncryptedNumber model_prediction = encrypted_model(row_features, encryption_manager.encrypt(0.0));
					const EncryptedNumber model_error = model_prediction - real_value;

					EncryptedNumber inner_derivative = encryption_manager.encrypt(1.0);
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

	double LinearModel::compute_mean_square_error(const Dataframe<double> & dataframe) const {
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

	EncryptedNumber LinearModel::compute_mean_square_error(const Dataframe<EncryptedNumber> & dataframe, EncryptedNumber encrypted_zero) const {
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
}

#endif