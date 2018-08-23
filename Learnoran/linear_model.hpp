#ifndef _LINEAR_MODEL_HPP
#define _LINEAR_MODEL_HPP

#include <random>
#include <string>
#include <vector>
#include <unordered_map>
#include <iostream> // std::cout, only for debug

#include "polynomial.hpp"
#include "dataframe.hpp"

class LinearModel {
public:
	LinearModel() { }

	void fit(const Dataframe & dataframe, const unsigned short epochs, const double learning_rate) {
		initialize_model(dataframe);

		std::cout << "BEFORE TRAINING\n";
		print_model_coefficients();

		for (unsigned short epoch = 0; epoch < epochs; epoch++) {
			mse_gradient_descent(dataframe, learning_rate);
		}

		std::cout << "AFTER TRAINING\n";
		print_model_coefficients();
	}

private:
	Polynomial model;

	static double random_standard_normal() {
		std::mt19937 generator;
		std::normal_distribution<double> sn_distribution(0, 1);
		return sn_distribution(generator);
	}

	void initialize_model(const Dataframe & dataframe) {
		// construct a linear polynomial with random coefficients from the standard normal distribution
		const std::vector<std::string> variable_symbols = dataframe.get_feature_headers();

		for (const std::string & variable : variable_symbols) {
			model.add_term(random_standard_normal(), variable, 1);
		}
	}

	void print_model_coefficients() {
		const std::unordered_map<std::string, PolynomialTerm> terms = model.get_terms();

		for (auto term = terms.cbegin(); term != terms.cend(); term++) {
			std::cout << term->first << ": " << term->second.coefficient << std::endl;
		}
	}

	void mse_gradient_descent(const Dataframe & dataframe, const double learning_rate) {
		// applies gradient descent to MSE cost function

		DataframeShape shape = dataframe.shape();

		for (std::pair<std::string, PolynomialTerm> term : model.get_terms()) {
			const std::string current_parameter = term.first;

			double derivative_mse_sum = 0;
			for (unsigned row = 0; row < shape.rows; row++) {
				const double real_value = dataframe.get_row_label(row);
				std::unordered_map<std::string, double> row_features = dataframe.get_row_feature(row);

				double derivative_mse = 2 * (real_value - model(row_features));
				derivative_mse *= model.partial_derivative(current_parameter, dataframe.get_row_feature(row)[current_parameter]);

				derivative_mse_sum += (1.0 / shape.rows) * derivative_mse;
			}
			double parameter_new_value = model[current_parameter] - learning_rate * derivative_mse_sum;
			model[current_parameter] = parameter_new_value;
		}
	}
};

#endif